/* CBFS Image Manipulation */
/* SPDX-License-Identifier: GPL-2.0-only */

#include <inttypes.h>
#include <libgen.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <commonlib/endian.h>
#include <vb2_sha.h>

#include "common.h"
#include "cbfs_image.h"
#include "elfparsing.h"
#include "rmodule.h"

/* Even though the file-adding functions---cbfs_add_entry() and
 * cbfs_add_entry_at()---perform their sizing checks against the beginning of
 * the subsequent section rather than a stable recorded value such as an empty
 * file header's len field, it's possible to prove two interesting properties
 * about their behavior:
 *  - Placing a new file within an empty entry located below an existing file
 *    entry will never leave an aligned flash address containing neither the
 *    beginning of a file header nor part of a file.
 *  - Placing a new file in an empty entry at the very end of the image such
 *    that it fits, but leaves no room for a final header, is guaranteed not to
 *    change the total amount of space for entries, even if that new file is
 *    later removed from the CBFS.
 * These properties are somewhat nonobvious from the implementation, so the
 * reader is encouraged to blame this comment and examine the full proofs
 * in the commit message before making significant changes that would risk
 * removing said guarantees.
 */

static const char *lookup_name_by_type(const struct typedesc_t *desc, uint32_t type,
				const char *default_value)
{
	int i;
	for (i = 0; desc[i].name; i++)
		if (desc[i].type == type)
			return desc[i].name;
	return default_value;
}

static int lookup_type_by_name(const struct typedesc_t *desc, const char *name)
{
	int i;
	for (i = 0; desc[i].name && strcasecmp(name, desc[i].name); ++i);
	return desc[i].name ? (int)desc[i].type : -1;
}

static const char *get_cbfs_entry_type_name(uint32_t type)
{
	return lookup_name_by_type(filetypes, type, "(unknown)");
}

int cbfs_parse_comp_algo(const char *name)
{
	return lookup_type_by_name(types_cbfs_compression, name);
}

/* CBFS image */

size_t cbfs_calculate_file_header_size(const char *name)
{
	return (sizeof(struct cbfs_file) +
		align_up(strlen(name) + 1, CBFS_ATTRIBUTE_ALIGN));
}

/* Only call on legacy CBFSes possessing a master header. */
static int cbfs_fix_legacy_size(struct cbfs_image *image, char *hdr_loc)
{
	assert(image);
	assert(cbfs_is_legacy_cbfs(image));
	// A bug in old cbfstool may produce extra few bytes (by alignment) and
	// cause cbfstool to overwrite things after free space -- which is
	// usually CBFS header on x86. We need to workaround that.
	// Except when we run across a file that contains the actual header,
	// in which case this image is a safe, new-style
	// `cbfstool add-master-header` based image.

	struct cbfs_file *entry, *first = NULL, *last = NULL;
	for (first = entry = cbfs_find_first_entry(image);
	     entry && cbfs_is_valid_entry(image, entry);
	     entry = cbfs_find_next_entry(image, entry)) {
		/* Is the header guarded by a CBFS file entry? Then exit */
		if (((char *)entry) + be32toh(entry->offset) == hdr_loc)
			return 0;
		last = entry;
	}
	if ((char *)first < (char *)hdr_loc &&
	    (char *)entry > (char *)hdr_loc) {
		WARN("CBFS image was created with old cbfstool with size bug. "
		     "Fixing size in last entry...\n");
		last->len = htobe32(be32toh(last->len) - image->header.align);
		DEBUG("Last entry has been changed from 0x%x to 0x%x.\n",
		      cbfs_get_entry_addr(image, entry),
		      cbfs_get_entry_addr(image,
					  cbfs_find_next_entry(image, last)));
	}
	return 0;
}

void cbfs_put_header(void *dest, const struct cbfs_header *header)
{
	struct buffer outheader;

	outheader.data = dest;
	outheader.size = 0;

	xdr_be.put32(&outheader, header->magic);
	xdr_be.put32(&outheader, header->version);
	xdr_be.put32(&outheader, header->romsize);
	xdr_be.put32(&outheader, header->bootblocksize);
	xdr_be.put32(&outheader, header->align);
	xdr_be.put32(&outheader, header->offset);
	xdr_be.put32(&outheader, header->architecture);
}

static void cbfs_decode_payload_segment(struct cbfs_payload_segment *output,
					struct cbfs_payload_segment *input)
{
	struct buffer seg = {
		.data = (void *)input,
		.size = sizeof(*input),
	};
	output->type = xdr_be.get32(&seg);
	output->compression = xdr_be.get32(&seg);
	output->offset = xdr_be.get32(&seg);
	output->load_addr = xdr_be.get64(&seg);
	output->len = xdr_be.get32(&seg);
	output->mem_len = xdr_be.get32(&seg);
	assert(seg.size == 0);
}

static int cbfs_file_get_compression_info(struct cbfs_file *entry,
	uint32_t *decompressed_size)
{
	unsigned int compression = CBFS_COMPRESS_NONE;
	if (decompressed_size)
		*decompressed_size = be32toh(entry->len);
	for (struct cbfs_file_attribute *attr = cbfs_file_first_attr(entry);
	     attr != NULL;
	     attr = cbfs_file_next_attr(entry, attr)) {
		if (be32toh(attr->tag) == CBFS_FILE_ATTR_TAG_COMPRESSION) {
			struct cbfs_file_attr_compression *ac =
				(struct cbfs_file_attr_compression *)attr;
			compression = be32toh(ac->compression);
			if (decompressed_size)
				*decompressed_size =
					be32toh(ac->decompressed_size);
		}
	}
	return compression;
}

static struct cbfs_file_attr_hash *cbfs_file_get_next_hash(
	struct cbfs_file *entry, struct cbfs_file_attr_hash *cur)
{
	struct cbfs_file_attribute *attr = (struct cbfs_file_attribute *)cur;
	if (attr == NULL) {
		attr = cbfs_file_first_attr(entry);
		if (attr == NULL)
			return NULL;
		if (be32toh(attr->tag) == CBFS_FILE_ATTR_TAG_HASH)
			return (struct cbfs_file_attr_hash *)attr;
	}
	while ((attr = cbfs_file_next_attr(entry, attr)) != NULL) {
		if (be32toh(attr->tag) == CBFS_FILE_ATTR_TAG_HASH)
			return (struct cbfs_file_attr_hash *)attr;
	};
	return NULL;
}

void cbfs_get_header(struct cbfs_header *header, void *src)
{
	struct buffer outheader;

	outheader.data = src;	/* We're not modifying the data */
	outheader.size = 0;

	header->magic = xdr_be.get32(&outheader);
	header->version = xdr_be.get32(&outheader);
	header->romsize = xdr_be.get32(&outheader);
	header->bootblocksize = xdr_be.get32(&outheader);
	header->align = xdr_be.get32(&outheader);
	header->offset = xdr_be.get32(&outheader);
	header->architecture = xdr_be.get32(&outheader);
}

int cbfs_image_create(struct cbfs_image *image, size_t entries_size)
{
	assert(image);
	assert(image->buffer.data);

	size_t empty_header_len = cbfs_calculate_file_header_size("");
	uint32_t entries_offset = 0;
	uint32_t align = CBFS_ALIGNMENT;
	if (image->has_header) {
		entries_offset = image->header.offset;

		if (entries_offset > image->buffer.size) {
			ERROR("CBFS file entries are located outside CBFS itself\n");
			return -1;
		}

		align = image->header.align;
	}

	// This attribute must be given in order to prove that this module
	// correctly preserves certain CBFS properties. See the block comment
	// near the top of this file (and the associated commit message).
	if (align < empty_header_len) {
		ERROR("CBFS must be aligned to at least %zu bytes\n",
							empty_header_len);
		return -1;
	}

	if (entries_size > image->buffer.size - entries_offset) {
		ERROR("CBFS doesn't have enough space to fit its file entries\n");
		return -1;
	}

	if (empty_header_len > entries_size) {
		ERROR("CBFS is too small to fit any header\n");
		return -1;
	}
	struct cbfs_file *entry_header =
		(struct cbfs_file *)(image->buffer.data + entries_offset);
	// This alignment is necessary in order to prove that this module
	// correctly preserves certain CBFS properties. See the block comment
	// near the top of this file (and the associated commit message).
	entries_size -= entries_size % align;

	size_t capacity = entries_size - empty_header_len;
	LOG("Created CBFS (capacity = %zu bytes)\n", capacity);
	return cbfs_create_empty_entry(entry_header, CBFS_TYPE_NULL,
		capacity, "");
}

int cbfs_legacy_image_create(struct cbfs_image *image,
			     uint32_t architecture,
			     uint32_t align,
			     struct buffer *bootblock,
			     uint32_t bootblock_offset,
			     uint32_t header_offset,
			     uint32_t entries_offset)
{
	assert(image);
	assert(image->buffer.data);
	assert(bootblock);

	int32_t *rel_offset;
	uint32_t cbfs_len;
	void *header_loc;
	size_t size = image->buffer.size;

	DEBUG("cbfs_image_create: bootblock=0x%x+0x%zx, "
	      "header=0x%x+0x%zx, entries_offset=0x%x\n",
	      bootblock_offset, bootblock->size, header_offset,
	      sizeof(image->header), entries_offset);

	DEBUG("cbfs_create_image: (real offset) bootblock=0x%x, "
	      "header=0x%x, entries_offset=0x%x\n",
	      bootblock_offset, header_offset, entries_offset);

	// Prepare bootblock
	if (bootblock_offset + bootblock->size > size) {
		ERROR("Bootblock (0x%x+0x%zx) exceed ROM size (0x%zx)\n",
		      bootblock_offset, bootblock->size, size);
		return -1;
	}
	if (entries_offset > bootblock_offset &&
	    entries_offset < bootblock->size) {
		ERROR("Bootblock (0x%x+0x%zx) overlap CBFS data (0x%x)\n",
		      bootblock_offset, bootblock->size, entries_offset);
		return -1;
	}
	memcpy(image->buffer.data + bootblock_offset, bootblock->data,
	       bootblock->size);

	// Prepare header
	if (header_offset + sizeof(image->header) > size - sizeof(int32_t)) {
		ERROR("Header (0x%x+0x%zx) exceed ROM size (0x%zx)\n",
		      header_offset, sizeof(image->header), size);
		return -1;
	}
	image->header.magic = CBFS_HEADER_MAGIC;
	image->header.version = CBFS_HEADER_VERSION;
	image->header.romsize = size;
	image->header.bootblocksize = bootblock->size;
	image->header.align = align;
	image->header.offset = entries_offset;
	image->header.architecture = architecture;

	header_loc = (image->buffer.data + header_offset);
	cbfs_put_header(header_loc, &image->header);
	image->has_header = true;

	// The last 4 byte of the image contain the relative offset from the end
	// of the image to the master header as a 32-bit signed integer. x86
	// relies on this also being its (memory-mapped, top-aligned) absolute
	// 32-bit address by virtue of how two's complement numbers work.
	assert(size % sizeof(int32_t) == 0);
	rel_offset = (int32_t *)(image->buffer.data + size - sizeof(int32_t));
	*rel_offset = header_offset - size;

	// Prepare entries
	if (align_up(entries_offset, align) != entries_offset) {
		ERROR("Offset (0x%x) must be aligned to 0x%x.\n",
		      entries_offset, align);
		return -1;
	}
	// To calculate available length, find
	//   e = min(bootblock, header, rel_offset) where e > entries_offset.
	cbfs_len = size - sizeof(int32_t);
	if (bootblock_offset > entries_offset && bootblock_offset < cbfs_len)
		cbfs_len = bootblock_offset;
	if (header_offset > entries_offset && header_offset < cbfs_len)
		cbfs_len = header_offset;

	if (cbfs_image_create(image, cbfs_len - entries_offset))
		return -1;
	return 0;
}

int cbfs_image_from_buffer(struct cbfs_image *out, struct buffer *in,
			   uint32_t offset)
{
	assert(out);
	assert(in);
	assert(in->data);

	buffer_clone(&out->buffer, in);
	out->has_header = false;

	if (cbfs_is_valid_cbfs(out)) {
		return 0;
	}

	void *header_loc = cbfs_find_header(in->data, in->size, offset);
	if (header_loc) {
		cbfs_get_header(&out->header, header_loc);
		out->has_header = true;
		cbfs_fix_legacy_size(out, header_loc);
		return 0;
	} else if (offset != ~0u) {
		ERROR("The -H switch is only valid on legacy images having CBFS master headers.\n");
		return 1;
	}
	ERROR("Selected image region is not a valid CBFS.\n");
	return 1;
}

int cbfs_copy_instance(struct cbfs_image *image, struct buffer *dst)
{
	assert(image);

	struct cbfs_file *src_entry, *dst_entry;
	size_t align;
	ssize_t last_entry_size;

	size_t copy_end = buffer_size(dst);

	align = CBFS_ALIGNMENT;

	dst_entry = (struct cbfs_file *)buffer_get(dst);

	/* Copy non-empty files */
	for (src_entry = cbfs_find_first_entry(image);
	     src_entry && cbfs_is_valid_entry(image, src_entry);
	     src_entry = cbfs_find_next_entry(image, src_entry)) {
		size_t entry_size;

		if ((src_entry->type == htobe32(CBFS_TYPE_NULL)) ||
		    (src_entry->type == htobe32(CBFS_TYPE_CBFSHEADER)) ||
		    (src_entry->type == htobe32(CBFS_TYPE_DELETED)))
			continue;

		entry_size = htobe32(src_entry->len) + htobe32(src_entry->offset);
		memcpy(dst_entry, src_entry, entry_size);
		dst_entry = (struct cbfs_file *)(
			(uintptr_t)dst_entry + align_up(entry_size, align));

		if ((size_t)((uint8_t *)dst_entry - (uint8_t *)buffer_get(dst))
					>= copy_end) {
			ERROR("Ran out of room in copy region.\n");
			return 1;
		}
	}

	/* Last entry size is all the room above it, except for top 4 bytes
	 * which may be used by the master header pointer. This messes with
	 * the ability to stash something "top-aligned" into the region, but
	 * keeps things simpler. */
	last_entry_size = copy_end -
		((uint8_t *)dst_entry - (uint8_t *)buffer_get(dst)) -
		cbfs_calculate_file_header_size("") - sizeof(int32_t);

	if (last_entry_size < 0)
		WARN("No room to create the last entry!\n")
	else
		cbfs_create_empty_entry(dst_entry, CBFS_TYPE_NULL,
			last_entry_size, "");

	return 0;
}

int cbfs_expand_to_region(struct buffer *region)
{
	if (buffer_get(region) == NULL)
		return 1;

	struct cbfs_image image;
	memset(&image, 0, sizeof(image));
	if (cbfs_image_from_buffer(&image, region, 0)) {
		ERROR("reading CBFS failed!\n");
		return 1;
	}

	uint32_t region_sz = buffer_size(region);

	struct cbfs_file *entry;
	for (entry = buffer_get(region);
	     cbfs_is_valid_entry(&image, entry);
	     entry = cbfs_find_next_entry(&image, entry)) {
	     /* just iterate through */
	}

	/* entry now points to the first aligned address after the last valid
	 * file header. That's either outside the image or exactly the place
	 * where we need to create a new file.
	 */
	int last_entry_size = region_sz -
		((uint8_t *)entry - (uint8_t *)buffer_get(region)) -
		cbfs_calculate_file_header_size("") - sizeof(int32_t);

	if (last_entry_size > 0) {
		cbfs_create_empty_entry(entry, CBFS_TYPE_NULL,
			last_entry_size, "");
		/* If the last entry was an empty file, merge them. */
		cbfs_legacy_walk(&image, cbfs_merge_empty_entry, NULL);
	}

	return 0;
}

int cbfs_truncate_space(struct buffer *region, uint32_t *size)
{
	if (buffer_get(region) == NULL)
		return 1;

	struct cbfs_image image;
	memset(&image, 0, sizeof(image));
	if (cbfs_image_from_buffer(&image, region, 0)) {
		ERROR("reading CBFS failed!\n");
		return 1;
	}

	struct cbfs_file *entry, *trailer;
	for (trailer = entry = buffer_get(region);
	     cbfs_is_valid_entry(&image, entry);
	     trailer = entry,
	     entry = cbfs_find_next_entry(&image, entry)) {
	     /* just iterate through */
	}

	/* trailer now points to the last valid CBFS entry's header.
	 * If that file is empty, remove it and report its header's offset as
	 * maximum size.
	 */
	if ((strlen(trailer->filename) != 0) &&
	    (trailer->type != htobe32(CBFS_TYPE_NULL)) &&
	    (trailer->type != htobe32(CBFS_TYPE_DELETED))) {
		/* nothing to truncate. Return de-facto CBFS size in case it
		 * was already truncated. */
		*size = (uint8_t *)entry - (uint8_t *)buffer_get(region);
		return 0;
	}
	*size = (uint8_t *)trailer - (uint8_t *)buffer_get(region);
	memset(trailer, 0xff, buffer_size(region) - *size);

	return 0;
}

static size_t cbfs_file_entry_metadata_size(const struct cbfs_file *f)
{
	return be32toh(f->offset);
}

static size_t cbfs_file_entry_data_size(const struct cbfs_file *f)
{
	return be32toh(f->len);
}

static size_t cbfs_file_entry_size(const struct cbfs_file *f)
{
	return cbfs_file_entry_metadata_size(f) + cbfs_file_entry_data_size(f);
}

int cbfs_compact_instance(struct cbfs_image *image)
{
	assert(image);

	struct cbfs_file *prev;
	struct cbfs_file *cur;

	/* The prev entry will always be an empty entry. */
	prev = NULL;

	/*
	 * Note: this function does not honor alignment or fixed location files.
	 * It's behavior is akin to cbfs_copy_instance() in that it expects
	 * the caller to understand the ramifications of compacting a
	 * fragmented CBFS image.
	 */

	for (cur = cbfs_find_first_entry(image);
	     cur && cbfs_is_valid_entry(image, cur);
	     cur = cbfs_find_next_entry(image, cur)) {
		size_t prev_size;
		size_t cur_size;
		size_t empty_metadata_size;
		size_t spill_size;

		/* Current entry is empty. Kepp track of it. */
		if (cur->type == CBFS_TYPE_NULL || cur->type == CBFS_TYPE_DELETED) {
			prev = cur;
			continue;
		}

		/* Need to ensure the previous entry is an empty one. */
		if (prev == NULL)
			continue;

		/* At this point prev is an empty entry. Put the non-empty
		 * file in prev's location. Then add a new empty entry. This
		 * essentialy bubbles empty entries towards the end. */

		prev_size = cbfs_file_entry_size(prev);
		cur_size = cbfs_file_entry_size(cur);

		/*
		 * Adjust the empty file size by the actual space occupied
		 * bewtween the beginning of the empty file and the non-empty
		 * file.
		 */
		prev_size += (cbfs_get_entry_addr(image, cur) -
				cbfs_get_entry_addr(image, prev)) - prev_size;

		/* Move the non-empty file over the empty file. */
		memmove(prev, cur, cur_size);

		/*
		 * Get location of the empty file. Note that since prev was
		 * overwritten with the non-empty file the previously moved
		 * file needs to be used to calculate the empty file's location.
		 */
		cur = cbfs_find_next_entry(image, prev);

		/*
		 * The total space to work with for swapping the 2 entries
		 * consists of the 2 files' sizes combined. However, the
		 * cbfs_file entries start on CBFS_ALIGNMENT boundaries.
		 * Because of this the empty file size may end up smaller
		 * because of the non-empty file's metadata and data length.
		 *
		 * Calculate the spill size which is the amount of data lost
		 * due to the alignment constraints after moving the non-empty
		 * file.
		 */
		spill_size = (cbfs_get_entry_addr(image, cur) -
				cbfs_get_entry_addr(image, prev)) - cur_size;

		empty_metadata_size = cbfs_calculate_file_header_size("");

		/* Check if new empty size can contain the metadata. */
		if (empty_metadata_size + spill_size > prev_size) {
			ERROR("Unable to swap '%s' with prev empty entry.\n",
				prev->filename);
			return 1;
		}

		/* Update the empty file's size. */
		prev_size -= spill_size + empty_metadata_size;

		/* Create new empty file. */
		cbfs_create_empty_entry(cur, CBFS_TYPE_NULL,
					prev_size, "");

		/* Merge any potential empty entries together. */
		cbfs_legacy_walk(image, cbfs_merge_empty_entry, NULL);

		/*
		 * Since current switched to an empty file keep track of it.
		 * Even if any empty files were merged the empty entry still
		 * starts at previously calculated location.
		 */
		prev = cur;
	}

	return 0;
}

int cbfs_image_delete(struct cbfs_image *image)
{
	if (image == NULL)
		return 0;

	buffer_delete(&image->buffer);
	return 0;
}

/* Tries to add an entry with its data (CBFS_SUBHEADER) at given offset. */
static int cbfs_add_entry_at(struct cbfs_image *image,
			     struct cbfs_file *entry,
			     const void *data,
			     uint32_t content_offset,
			     const struct cbfs_file *header,
			     const size_t len_align)
{
	struct cbfs_file *next = cbfs_find_next_entry(image, entry);
	uint32_t addr = cbfs_get_entry_addr(image, entry),
		 addr_next = cbfs_get_entry_addr(image, next);
	uint32_t min_entry_size = cbfs_calculate_file_header_size("");
	uint32_t len, header_offset;
	uint32_t align = image->has_header ? image->header.align :
							CBFS_ALIGNMENT;
	uint32_t header_size = be32toh(header->offset);

	header_offset = content_offset - header_size;
	if (header_offset % align)
		header_offset -= header_offset % align;
	if (header_offset < addr) {
		ERROR("No space to hold cbfs_file header.");
		return -1;
	}

	// Process buffer BEFORE content_offset.
	if (header_offset - addr > min_entry_size) {
		DEBUG("|min|...|header|content|... <create new entry>\n");
		len = header_offset - addr - min_entry_size;
		cbfs_create_empty_entry(entry, CBFS_TYPE_NULL, len, "");
		if (verbose > 1) cbfs_print_entry_info(image, entry, stderr);
		entry = cbfs_find_next_entry(image, entry);
		addr = cbfs_get_entry_addr(image, entry);
	}

	len = content_offset - addr - header_size;
	memcpy(entry, header, header_size);
	if (len != 0) {
		/*
		 * The header moved backwards a bit to accommodate cbfs_file
		 * alignment requirements, so patch up ->offset to still point
		 * to file data. Move attributes forward so the end of the
		 * attribute list still matches the end of the metadata.
		 */
		uint32_t offset = be32toh(entry->offset);
		uint32_t attrs = be32toh(entry->attributes_offset);
		DEBUG("|..|header|content|... <use offset to create entry>\n");
		DEBUG("before: attr_offset=0x%x, offset=0x%x\n", attrs, offset);
		if (attrs == 0) {
			memset((uint8_t *)entry + offset, 0, len);
		} else {
			uint8_t *p = (uint8_t *)entry + attrs;
			memmove(p + len, p, offset - attrs);
			memset(p, 0, len);
			attrs += len;
			entry->attributes_offset = htobe32(attrs);
		}
		offset += len;
		entry->offset = htobe32(offset);
		DEBUG("after: attr_offset=0x%x, offset=0x%x\n", attrs, offset);
	}

	// Ready to fill data into entry.
	DEBUG("content_offset: 0x%x, entry location: %x\n",
	      content_offset, (int)((char*)CBFS_SUBHEADER(entry) -
				    image->buffer.data));
	assert((char*)CBFS_SUBHEADER(entry) - image->buffer.data ==
	       (ptrdiff_t)content_offset);
	memcpy(CBFS_SUBHEADER(entry), data, be32toh(entry->len));
	if (verbose > 1) cbfs_print_entry_info(image, entry, stderr);

	// Align the length to a multiple of len_align
	if (len_align &&
	    ((be32toh(entry->offset) + be32toh(entry->len)) % len_align)) {
		size_t off = (be32toh(entry->offset) + be32toh(entry->len)) % len_align;
		entry->len = htobe32(be32toh(entry->len) + len_align - off);
	}

	// Process buffer AFTER entry.
	entry = cbfs_find_next_entry(image, entry);
	addr = cbfs_get_entry_addr(image, entry);
	if (addr == addr_next)
		return 0;

	assert(addr < addr_next);
	if (addr_next - addr < min_entry_size) {
		DEBUG("No need for new \"empty\" entry\n");
		/* No need to increase the size of the just
		 * stored file to extend to next file. Alignment
		 * of next file takes care of this.
		 */
		return 0;
	}

	len = addr_next - addr - min_entry_size;
	/* keep space for master header pointer */
	if ((uint8_t *)entry + min_entry_size + len >
			(uint8_t *)buffer_get(&image->buffer) +
			buffer_size(&image->buffer) - sizeof(int32_t)) {
		len -= sizeof(int32_t);
	}
	cbfs_create_empty_entry(entry, CBFS_TYPE_NULL, len, "");
	if (verbose > 1) cbfs_print_entry_info(image, entry, stderr);
	return 0;
}

int cbfs_add_entry(struct cbfs_image *image, struct buffer *buffer,
		   uint32_t content_offset,
		   struct cbfs_file *header,
		   const size_t len_align)
{
	assert(image);
	assert(buffer);
	assert(buffer->data);
	assert(!IS_HOST_SPACE_ADDRESS(content_offset));

	const char *name = header->filename;

	uint32_t entry_type;
	uint32_t addr, addr_next;
	struct cbfs_file *entry, *next;
	uint32_t need_size;
	uint32_t header_size = be32toh(header->offset);

	need_size = header_size + buffer->size;
	DEBUG("cbfs_add_entry('%s'@0x%x) => need_size = %u+%zu=%u\n",
	      name, content_offset, header_size, buffer->size, need_size);

	// Merge empty entries.
	DEBUG("(trying to merge empty entries...)\n");
	cbfs_legacy_walk(image, cbfs_merge_empty_entry, NULL);

	for (entry = cbfs_find_first_entry(image);
	     entry && cbfs_is_valid_entry(image, entry);
	     entry = cbfs_find_next_entry(image, entry)) {

		entry_type = be32toh(entry->type);
		if (entry_type != CBFS_TYPE_NULL)
			continue;

		addr = cbfs_get_entry_addr(image, entry);
		next = cbfs_find_next_entry(image, entry);
		addr_next = cbfs_get_entry_addr(image, next);

		DEBUG("cbfs_add_entry: space at 0x%x+0x%x(%d) bytes\n",
		      addr, addr_next - addr, addr_next - addr);

		/* Will the file fit? Don't yet worry if we have space for a new
		 * "empty" entry. We take care of that later.
		 */
		if (addr + need_size > addr_next)
			continue;

		// Test for complicated cases
		if (content_offset > 0) {
			if (addr_next < content_offset) {
				DEBUG("Not for specified offset yet");
				continue;
			} else if (addr > content_offset) {
				DEBUG("Exceed specified content_offset.");
				break;
			} else if (addr + header_size > content_offset) {
				ERROR("Not enough space for header.\n");
				break;
			} else if (content_offset + buffer->size > addr_next) {
				ERROR("Not enough space for content.\n");
				break;
			}
		}

		// TODO there are more few tricky cases that we may
		// want to fit by altering offset.

		if (content_offset == 0) {
			// we tested every condition earlier under which
			// placing the file there might fail
			content_offset = addr + header_size;
		}

		DEBUG("section 0x%x+0x%x for content_offset 0x%x.\n",
		      addr, addr_next - addr, content_offset);

		if (cbfs_add_entry_at(image, entry, buffer->data,
				      content_offset, header, len_align) == 0) {
			return 0;
		}
		break;
	}

	ERROR("Could not add [%s, %zd bytes (%zd KB)@0x%x]; too big?\n",
	      buffer->name, buffer->size, buffer->size / 1024, content_offset);
	return -1;
}

struct cbfs_file *cbfs_get_entry(struct cbfs_image *image, const char *name)
{
	struct cbfs_file *entry;
	for (entry = cbfs_find_first_entry(image);
	     entry && cbfs_is_valid_entry(image, entry);
	     entry = cbfs_find_next_entry(image, entry)) {
		if (strcasecmp(entry->filename, name) == 0) {
			DEBUG("cbfs_get_entry: found %s\n", name);
			return entry;
		}
	}
	return NULL;
}

static int cbfs_payload_decompress(struct cbfs_payload_segment *segments,
		struct buffer *buff, int num_seg)
{
	struct buffer new_buffer;
	struct buffer seg_buffer;
	size_t new_buff_sz;
	char *in_ptr;
	char *out_ptr;
	size_t new_offset;
	decomp_func_ptr decompress;

	new_offset = num_seg * sizeof(*segments);
	new_buff_sz = num_seg * sizeof(*segments);

	/* Find out and allocate the amount of memory occupied
	 * by the binary data */
	for (int i = 0; i < num_seg; i++)
		new_buff_sz += segments[i].mem_len;

	if (buffer_create(&new_buffer, new_buff_sz, "decompressed_buff"))
		return -1;

	in_ptr = buffer_get(buff) + new_offset;
	out_ptr = buffer_get(&new_buffer) + new_offset;

	for (int i = 0; i < num_seg; i++) {
		struct buffer tbuff;
		size_t decomp_size;

		/* Segments BSS and ENTRY do not have binary data. */
		if (segments[i].type == PAYLOAD_SEGMENT_BSS ||
				segments[i].type == PAYLOAD_SEGMENT_ENTRY) {
			continue;
		} else if (segments[i].type == PAYLOAD_SEGMENT_PARAMS) {
			memcpy(out_ptr, in_ptr, segments[i].len);
			segments[i].offset = new_offset;
			new_offset += segments[i].len;
			in_ptr += segments[i].len;
			out_ptr += segments[i].len;
			segments[i].compression = CBFS_COMPRESS_NONE;
			continue;
		}

		/* The payload uses an unknown compression algorithm. */
		decompress = decompression_function(segments[i].compression);
		if (decompress == NULL) {
			ERROR("Unknown decompression algorithm: %u\n",
					segments[i].compression);
			return -1;
		}

		if (buffer_create(&tbuff, segments[i].mem_len, "segment")) {
			buffer_delete(&new_buffer);
			return -1;
		}

		if (decompress(in_ptr, segments[i].len, buffer_get(&tbuff),
					(int) buffer_size(&tbuff),
					&decomp_size)) {
			ERROR("Couldn't decompress payload segment %u\n", i);
			buffer_delete(&new_buffer);
			buffer_delete(&tbuff);
			return -1;
		}

		memcpy(out_ptr, buffer_get(&tbuff), decomp_size);

		in_ptr += segments[i].len;

		/* Update the offset of the segment. */
		segments[i].offset = new_offset;
		/* True decompressed size is just the data size. No metadata */
		segments[i].len = decomp_size;
		/* Segment is not compressed. */
		segments[i].compression = CBFS_COMPRESS_NONE;

		/* Update the offset and output buffer pointer. */
		new_offset += decomp_size;
		out_ptr += decomp_size;

		buffer_delete(&tbuff);
	}

	buffer_splice(&seg_buffer, &new_buffer, 0, 0);
	xdr_segs(&seg_buffer, segments, num_seg);

	buffer_delete(buff);
	*buff = new_buffer;

	return 0;
}

static int init_elf_from_arch(Elf64_Ehdr *ehdr, uint32_t cbfs_arch)
{
	int endian;
	int nbits;
	int machine;

	switch (cbfs_arch) {
	case CBFS_ARCHITECTURE_X86:
		endian = ELFDATA2LSB;
		nbits = ELFCLASS32;
		machine = EM_386;
		break;
	case CBFS_ARCHITECTURE_ARM:
		endian = ELFDATA2LSB;
		nbits = ELFCLASS32;
		machine = EM_ARM;
		break;
	case CBFS_ARCHITECTURE_AARCH64:
		endian = ELFDATA2LSB;
		nbits = ELFCLASS64;
		machine = EM_AARCH64;
		break;
	case CBFS_ARCHITECTURE_MIPS:
		endian = ELFDATA2LSB;
		nbits = ELFCLASS32;
		machine = EM_MIPS;
		break;
	case CBFS_ARCHITECTURE_RISCV:
		endian = ELFDATA2LSB;
		nbits = ELFCLASS32;
		machine = EM_RISCV;
		break;
	default:
		ERROR("Unsupported arch: %x\n", cbfs_arch);
		return -1;
	}

	elf_init_eheader(ehdr, machine, nbits, endian);
	return 0;
}

static int cbfs_stage_make_elf(struct buffer *buff, uint32_t arch,
			       struct cbfs_file *entry)
{
	Elf64_Ehdr ehdr;
	Elf64_Shdr shdr;
	struct elf_writer *ew;
	struct buffer elf_out;
	size_t empty_sz;
	int rmod_ret;

	if (arch == CBFS_ARCHITECTURE_UNKNOWN) {
		ERROR("You need to specify -m ARCH.\n");
		return -1;
	}

	struct cbfs_file_attr_stageheader *stage = NULL;
	for (struct cbfs_file_attribute *attr = cbfs_file_first_attr(entry);
	     attr != NULL; attr = cbfs_file_next_attr(entry, attr)) {
		if (be32toh(attr->tag) == CBFS_FILE_ATTR_TAG_STAGEHEADER) {
			stage = (struct cbfs_file_attr_stageheader *)attr;
			break;
		}
	}

	if (stage == NULL) {
		ERROR("Stage header not found for %s\n", entry->filename);
		return -1;
	}

	if (init_elf_from_arch(&ehdr, arch))
		return -1;

	/* Attempt rmodule translation first. */
	rmod_ret = rmodule_stage_to_elf(&ehdr, buff);

	if (rmod_ret < 0) {
		ERROR("rmodule parsing failed\n");
		return -1;
	} else if (rmod_ret == 0)
		return 0;

	/* Rmodule couldn't do anything with the data. Continue on with SELF. */

	ehdr.e_entry = be64toh(stage->loadaddr) + be32toh(stage->entry_offset);

	ew = elf_writer_init(&ehdr);
	if (ew == NULL) {
		ERROR("Unable to init ELF writer.\n");
		return -1;
	}

	memset(&shdr, 0, sizeof(shdr));
	shdr.sh_type = SHT_PROGBITS;
	shdr.sh_flags = SHF_WRITE | SHF_ALLOC | SHF_EXECINSTR;
	shdr.sh_addr = be64toh(stage->loadaddr);
	shdr.sh_size = buffer_size(buff);
	empty_sz = be32toh(stage->memlen) - buffer_size(buff);

	if (elf_writer_add_section(ew, &shdr, buff, ".program")) {
		ERROR("Unable to add ELF section: .program\n");
		elf_writer_destroy(ew);
		return -1;
	}

	if (empty_sz != 0) {
		struct buffer b;

		buffer_init(&b, NULL, NULL, 0);
		memset(&shdr, 0, sizeof(shdr));
		shdr.sh_type = SHT_NOBITS;
		shdr.sh_flags = SHF_WRITE | SHF_ALLOC;
		shdr.sh_addr = be64toh(stage->loadaddr) + buffer_size(buff);
		shdr.sh_size = empty_sz;
		if (elf_writer_add_section(ew, &shdr, &b, ".empty")) {
			ERROR("Unable to add ELF section: .empty\n");
			elf_writer_destroy(ew);
			return -1;
		}
	}

	if (elf_writer_serialize(ew, &elf_out)) {
		ERROR("Unable to create ELF file from stage.\n");
		elf_writer_destroy(ew);
		return -1;
	}

	/* Flip buffer with the created ELF one. */
	buffer_delete(buff);
	*buff = elf_out;

	elf_writer_destroy(ew);

	return 0;
}

static int cbfs_payload_make_elf(struct buffer *buff, uint32_t arch,
				 unused struct cbfs_file *entry)
{
	Elf64_Ehdr ehdr;
	Elf64_Shdr shdr;
	struct cbfs_payload_segment *segs = NULL;
	struct elf_writer *ew = NULL;
	struct buffer elf_out;
	int segments = 0;
	int retval = -1;

	if (arch == CBFS_ARCHITECTURE_UNKNOWN) {
		ERROR("You need to specify -m ARCH.\n");
		goto out;
	}

	/* Count the number of segments inside buffer */
	while (true) {
		uint32_t payload_type = 0;

		struct cbfs_payload_segment *seg;

		seg = buffer_get(buff);
		payload_type = read_be32(&seg[segments].type);

		if (payload_type == PAYLOAD_SEGMENT_CODE) {
			segments++;
		} else if (payload_type == PAYLOAD_SEGMENT_DATA) {
			segments++;
		} else if (payload_type == PAYLOAD_SEGMENT_BSS) {
			segments++;
		} else if (payload_type == PAYLOAD_SEGMENT_PARAMS) {
			segments++;
		} else if (payload_type == PAYLOAD_SEGMENT_ENTRY) {
			/* The last segment in a payload is always ENTRY as
			 * specified by the  parse_elf_to_payload() function.
			 * Therefore there is no need to continue looking for
			 * segments.*/
			segments++;
			break;
		} else {
			ERROR("Unknown payload segment type: %x\n",
					payload_type);
			goto out;
		}
	}

	segs = malloc(segments * sizeof(*segs));

	/* Decode xdr segments */
	for (int i = 0; i < segments; i++) {
		struct cbfs_payload_segment *serialized_seg = buffer_get(buff);
		xdr_get_seg(&segs[i], &serialized_seg[i]);
	}

	if (cbfs_payload_decompress(segs, buff, segments)) {
		ERROR("Failed to decompress payload.\n");
		goto out;
	}

	if (init_elf_from_arch(&ehdr, arch))
		goto out;

	ehdr.e_entry = segs[segments-1].load_addr;

	ew = elf_writer_init(&ehdr);
	if (ew == NULL) {
		ERROR("Unable to init ELF writer.\n");
		goto out;
	}

	for (int i = 0; i < segments; i++) {
		struct buffer tbuff;
		size_t empty_sz = 0;

		memset(&shdr, 0, sizeof(shdr));
		char *name = NULL;

		if (segs[i].type == PAYLOAD_SEGMENT_CODE) {
			shdr.sh_type = SHT_PROGBITS;
			shdr.sh_flags = SHF_WRITE | SHF_ALLOC | SHF_EXECINSTR;
			shdr.sh_addr = segs[i].load_addr;
			shdr.sh_size = segs[i].len;
			empty_sz = segs[i].mem_len - segs[i].len;
			name = strdup(".text");
			buffer_splice(&tbuff, buff, segs[i].offset,
				       segs[i].len);
		} else if (segs[i].type == PAYLOAD_SEGMENT_DATA) {
			shdr.sh_type = SHT_PROGBITS;
			shdr.sh_flags = SHF_ALLOC | SHF_WRITE;
			shdr.sh_addr = segs[i].load_addr;
			shdr.sh_size = segs[i].len;
			empty_sz = segs[i].mem_len - segs[i].len;
			name = strdup(".data");
			buffer_splice(&tbuff, buff, segs[i].offset,
				       segs[i].len);
		} else if (segs[i].type == PAYLOAD_SEGMENT_BSS) {
			shdr.sh_type = SHT_NOBITS;
			shdr.sh_flags = SHF_ALLOC | SHF_WRITE;
			shdr.sh_addr = segs[i].load_addr;
			shdr.sh_size = segs[i].len;
			name = strdup(".bss");
			buffer_splice(&tbuff, buff, 0, 0);
		} else if (segs[i].type == PAYLOAD_SEGMENT_PARAMS) {
			shdr.sh_type = SHT_NOTE;
			shdr.sh_flags = 0;
			shdr.sh_size = segs[i].len;
			name = strdup(".note.pinfo");
			buffer_splice(&tbuff, buff, segs[i].offset,
				       segs[i].len);
		} else if (segs[i].type == PAYLOAD_SEGMENT_ENTRY) {
			break;
		} else {
			ERROR("unknown ELF segment type\n");
			goto out;
		}

		if (!name) {
			ERROR("out of memory\n");
			goto out;
		}

		if (elf_writer_add_section(ew, &shdr, &tbuff, name)) {
			ERROR("Unable to add ELF section: %s\n", name);
			free(name);
			goto out;
		}
		free(name);

		if (empty_sz != 0) {
			struct buffer b;

			buffer_init(&b, NULL, NULL, 0);
			memset(&shdr, 0, sizeof(shdr));
			shdr.sh_type = SHT_NOBITS;
			shdr.sh_flags = SHF_WRITE | SHF_ALLOC;
			shdr.sh_addr = segs[i].load_addr + segs[i].len;
			shdr.sh_size = empty_sz;
			name = strdup(".empty");
			if (!name) {
				ERROR("out of memory\n");
				goto out;
			}
			if (elf_writer_add_section(ew, &shdr, &b, name)) {
				ERROR("Unable to add ELF section: %s\n", name);
				free(name);
				goto out;
			}
			free(name);
		}
	}

	if (elf_writer_serialize(ew, &elf_out)) {
		ERROR("Unable to create ELF file from payload.\n");
		goto out;
	}

	/* Flip buffer with the created ELF one. */
	buffer_delete(buff);
	*buff = elf_out;
	retval = 0;

out:
	free(segs);
	elf_writer_destroy(ew);
	return retval;
}

int cbfs_export_entry(struct cbfs_image *image, const char *entry_name,
		      const char *filename, uint32_t arch, bool do_processing)
{
	struct cbfs_file *entry = cbfs_get_entry(image, entry_name);
	struct buffer buffer;
	if (!entry) {
		ERROR("File not found: %s\n", entry_name);
		return -1;
	}

	unsigned int compressed_size = be32toh(entry->len);
	unsigned int decompressed_size = 0;
	unsigned int compression = cbfs_file_get_compression_info(entry,
		&decompressed_size);
	unsigned int buffer_len;
	decomp_func_ptr decompress;

	if (do_processing) {
		decompress = decompression_function(compression);
		if (!decompress) {
			ERROR("looking up decompression routine failed\n");
			return -1;
		}
		buffer_len = decompressed_size;
	} else {
		/* Force nop decompression */
		decompress = decompression_function(CBFS_COMPRESS_NONE);
		buffer_len = compressed_size;
	}

	LOG("Found file %.30s at 0x%x, type %.12s, compressed %d, size %d\n",
	    entry_name, cbfs_get_entry_addr(image, entry),
	    get_cbfs_entry_type_name(be32toh(entry->type)), compressed_size,
	    decompressed_size);

	buffer_init(&buffer, strdup("(cbfs_export_entry)"), NULL, 0);
	buffer.data = malloc(buffer_len);
	buffer.size = buffer_len;

	if (decompress(CBFS_SUBHEADER(entry), compressed_size,
		       buffer.data, buffer.size, NULL)) {
		ERROR("decompression failed for %s\n", entry_name);
		buffer_delete(&buffer);
		return -1;
	}

	/*
	 * We want to export stages and payloads as ELFs, not with coreboot's
	 * custom stage/SELF binary formats, so we need to do extra processing
	 * to turn them back into an ELF.
	 */
	if (do_processing) {
		int (*make_elf)(struct buffer *, uint32_t,
				struct cbfs_file *) = NULL;
		switch (be32toh(entry->type)) {
		case CBFS_TYPE_STAGE:
			make_elf = cbfs_stage_make_elf;
			break;
		case CBFS_TYPE_SELF:
			make_elf = cbfs_payload_make_elf;
			break;
		}
		if (make_elf && make_elf(&buffer, arch, entry)) {
			ERROR("Failed to write %s into %s.\n",
			      entry_name, filename);
			buffer_delete(&buffer);
			return -1;
		}
	}

	if (buffer_write_file(&buffer, filename) != 0) {
		ERROR("Failed to write %s into %s.\n",
		      entry_name, filename);
		buffer_delete(&buffer);
		return -1;
	}

	buffer_delete(&buffer);
	INFO("Successfully dumped the file to: %s\n", filename);
	return 0;
}

int cbfs_remove_entry(struct cbfs_image *image, const char *name)
{
	struct cbfs_file *entry;
	entry = cbfs_get_entry(image, name);
	if (!entry) {
		ERROR("CBFS file %s not found.\n", name);
		return -1;
	}
	DEBUG("cbfs_remove_entry: Removed %s @ 0x%x\n",
	      entry->filename, cbfs_get_entry_addr(image, entry));
	entry->type = htobe32(CBFS_TYPE_DELETED);
	cbfs_legacy_walk(image, cbfs_merge_empty_entry, NULL);
	return 0;
}

int cbfs_print_header_info(struct cbfs_image *image)
{
	char *name = strdup(image->buffer.name);
	assert(image);
	printf("%s: %zd kB, bootblocksize %d, romsize %d, offset 0x%x\n"
	       "alignment: %d bytes, architecture: %s\n\n",
	       basename(name),
	       image->buffer.size / 1024,
	       image->header.bootblocksize,
	       image->header.romsize,
	       image->header.offset,
	       image->header.align,
	       arch_to_string(image->header.architecture));
	free(name);
	return 0;
}

static int cbfs_print_stage_info(struct cbfs_file *entry, FILE* fp)
{

	struct cbfs_file_attr_stageheader *stage = NULL;
	for (struct cbfs_file_attribute *attr = cbfs_file_first_attr(entry);
	     attr != NULL; attr = cbfs_file_next_attr(entry, attr)) {
		if (be32toh(attr->tag) == CBFS_FILE_ATTR_TAG_STAGEHEADER) {
			stage = (struct cbfs_file_attr_stageheader *)attr;
			break;
		}
	}

	if (stage == NULL) {
		fprintf(fp, "    ERROR: stage header not found!\n");
		return -1;
	}

	fprintf(fp,
		"    entry: 0x%" PRIx64 ", load: 0x%" PRIx64 ", "
		"memlen: %d\n",
		be64toh(stage->loadaddr) + be32toh(stage->entry_offset),
		be64toh(stage->loadaddr),
		be32toh(stage->memlen));
	return 0;
}

static int cbfs_print_decoded_payload_segment_info(
		struct cbfs_payload_segment *seg, FILE *fp)
{
	/* The input (seg) must be already decoded by
	 * cbfs_decode_payload_segment.
	 */
	switch (seg->type) {
		case PAYLOAD_SEGMENT_CODE:
		case PAYLOAD_SEGMENT_DATA:
			fprintf(fp, "    %s (%s compression, offset: 0x%x, "
				"load: 0x%" PRIx64 ", length: %d/%d)\n",
				(seg->type == PAYLOAD_SEGMENT_CODE ?
				 "code " : "data"),
				lookup_name_by_type(types_cbfs_compression,
						    seg->compression,
						    "(unknown)"),
				seg->offset, seg->load_addr, seg->len,
				seg->mem_len);
			break;

		case PAYLOAD_SEGMENT_ENTRY:
			fprintf(fp, "    entry (0x%" PRIx64 ")\n",
				seg->load_addr);
			break;

		case PAYLOAD_SEGMENT_BSS:
			fprintf(fp, "    BSS (address 0x%016" PRIx64 ", "
				"length 0x%x)\n",
				seg->load_addr, seg->len);
			break;

		case PAYLOAD_SEGMENT_PARAMS:
			fprintf(fp, "    parameters\n");
			break;

		default:
			fprintf(fp, "   0x%x (%s compression, offset: 0x%x, "
				"load: 0x%" PRIx64 ", length: %d/%d\n",
				seg->type,
				lookup_name_by_type(types_cbfs_compression,
						    seg->compression,
						    "(unknown)"),
				seg->offset, seg->load_addr, seg->len,
				seg->mem_len);
			break;
	}
	return 0;
}

int cbfs_print_entry_info(struct cbfs_image *image, struct cbfs_file *entry,
			  void *arg)
{
	const char *name = entry->filename;
	struct cbfs_payload_segment *payload;
	FILE *fp = (FILE *)arg;

	if (!cbfs_is_valid_entry(image, entry)) {
		ERROR("cbfs_print_entry_info: Invalid entry at 0x%x\n",
		      cbfs_get_entry_addr(image, entry));
		return -1;
	}
	if (!fp)
		fp = stdout;

	unsigned int decompressed_size = 0;
	unsigned int compression = cbfs_file_get_compression_info(entry,
		&decompressed_size);
	const char *compression_name = lookup_name_by_type(
			types_cbfs_compression, compression, "????");

	if (compression == CBFS_COMPRESS_NONE)
		fprintf(fp, "%-30s 0x%-8x %-12s %8d %-4s\n",
			*name ? name : "(empty)",
			cbfs_get_entry_addr(image, entry),
			get_cbfs_entry_type_name(be32toh(entry->type)),
			be32toh(entry->len),
			compression_name
			);
	else
		fprintf(fp, "%-30s 0x%-8x %-12s %8d %-4s (%d decompressed)\n",
			*name ? name : "(empty)",
			cbfs_get_entry_addr(image, entry),
			get_cbfs_entry_type_name(be32toh(entry->type)),
			be32toh(entry->len),
			compression_name,
			decompressed_size
			);

	if (!verbose)
		return 0;

	struct cbfs_file_attr_hash *attr = NULL;
	while ((attr = cbfs_file_get_next_hash(entry, attr)) != NULL) {
		size_t hash_len = vb2_digest_size(attr->hash.algo);
		if (!hash_len) {
			fprintf(fp, "invalid/unsupported hash algorithm: %d\n",
				attr->hash.algo);
			break;
		}
		char *hash_str = bintohex(attr->hash.raw, hash_len);
		int valid = vb2_hash_verify(CBFS_SUBHEADER(entry),
			be32toh(entry->len), &attr->hash) == VB2_SUCCESS;
		const char *valid_str = valid ? "valid" : "invalid";

		fprintf(fp, "    hash %s:%s %s\n",
			vb2_get_hash_algorithm_name(attr->hash.algo),
			hash_str, valid_str);
		free(hash_str);
	}

	DEBUG(" cbfs_file=0x%x, offset=0x%x, content_address=0x%x+0x%x\n",
	      cbfs_get_entry_addr(image, entry), be32toh(entry->offset),
	      cbfs_get_entry_addr(image, entry) + be32toh(entry->offset),
	      be32toh(entry->len));

	/* note the components of the subheader may be in host order ... */
	switch (be32toh(entry->type)) {
		case CBFS_TYPE_STAGE:
			cbfs_print_stage_info(entry, fp);
			break;

		case CBFS_TYPE_SELF:
			payload = (struct cbfs_payload_segment *)
					CBFS_SUBHEADER(entry);
			while (payload) {
				struct cbfs_payload_segment seg;
				cbfs_decode_payload_segment(&seg, payload);
				cbfs_print_decoded_payload_segment_info(
						&seg, fp);
				if (seg.type == PAYLOAD_SEGMENT_ENTRY)
					break;
				else
				payload ++;
			}
			break;
		default:
			break;
	}
	return 0;
}

static int cbfs_print_parseable_entry_info(struct cbfs_image *image,
					struct cbfs_file *entry, void *arg)
{
	FILE *fp = (FILE *)arg;
	const char *name;
	const char *type;
	size_t offset;
	size_t metadata_size;
	size_t data_size;
	const char *sep = "\t";

	if (!cbfs_is_valid_entry(image, entry)) {
		ERROR("cbfs_print_entry_info: Invalid entry at 0x%x\n",
		      cbfs_get_entry_addr(image, entry));
		return -1;
	}

	name = entry->filename;
	if (*name == '\0')
		name = "(empty)";
	type = get_cbfs_entry_type_name(be32toh(entry->type)),
	metadata_size = be32toh(entry->offset);
	data_size = be32toh(entry->len);
	offset = cbfs_get_entry_addr(image, entry);

	fprintf(fp, "%s%s", name, sep);
	fprintf(fp, "0x%zx%s", offset, sep);
	fprintf(fp, "%s%s", type, sep);
	fprintf(fp, "0x%zx%s", metadata_size, sep);
	fprintf(fp, "0x%zx%s", data_size, sep);
	fprintf(fp, "0x%zx", metadata_size + data_size);

	if (verbose) {
		unsigned int decompressed_size = 0;
		unsigned int compression = cbfs_file_get_compression_info(entry,
			&decompressed_size);
		if (compression != CBFS_COMPRESS_NONE)
			fprintf(fp, "%scomp:%s:0x%x", sep, lookup_name_by_type(
				types_cbfs_compression, compression, "????"),
				decompressed_size);

		struct cbfs_file_attr_hash *attr = NULL;
		while ((attr = cbfs_file_get_next_hash(entry, attr)) != NULL) {
			size_t hash_len = vb2_digest_size(attr->hash.algo);
			if (!hash_len)
				continue;
			char *hash_str = bintohex(attr->hash.raw, hash_len);
			int valid = vb2_hash_verify(CBFS_SUBHEADER(entry),
				be32toh(entry->len), &attr->hash) == VB2_SUCCESS;
			fprintf(fp, "%shash:%s:%s:%s", sep,
				vb2_get_hash_algorithm_name(attr->hash.algo),
				hash_str, valid ? "valid" : "invalid");
			free(hash_str);
		}
	}
	fprintf(fp, "\n");

	return 0;
}

void cbfs_print_directory(struct cbfs_image *image)
{
	if (cbfs_is_legacy_cbfs(image))
		cbfs_print_header_info(image);
	printf("%-30s %-10s %-12s   Size   Comp\n", "Name", "Offset", "Type");
	cbfs_legacy_walk(image, cbfs_print_entry_info, NULL);
}

void cbfs_print_parseable_directory(struct cbfs_image *image)
{
	size_t i;
	const char *header[] = {
		"Name",
		"Offset",
		"Type",
		"Metadata Size",
		"Data Size",
		"Total Size",
	};
	const char *sep = "\t";

	for (i = 0; i < ARRAY_SIZE(header) - 1; i++)
		fprintf(stdout, "%s%s", header[i], sep);
	fprintf(stdout, "%s\n", header[i]);
	cbfs_legacy_walk(image, cbfs_print_parseable_entry_info, stdout);
}

int cbfs_merge_empty_entry(struct cbfs_image *image, struct cbfs_file *entry,
			   unused void *arg)
{
	struct cbfs_file *next;
	uint32_t next_addr = 0;

	/* We don't return here even if this entry is already empty because we
	   want to merge the empty entries following after it. */

	/* Loop until non-empty entry is found, starting from the current entry.
	   After the loop, next_addr points to the next non-empty entry. */
	next = entry;
	while (be32toh(next->type) == CBFS_TYPE_DELETED ||
			be32toh(next->type) == CBFS_TYPE_NULL) {
		next = cbfs_find_next_entry(image, next);
		if (!next)
			break;
		next_addr = cbfs_get_entry_addr(image, next);
		if (!cbfs_is_valid_entry(image, next))
			/* 'next' could be the end of cbfs */
			break;
	}

	if (!next_addr)
		/* Nothing to empty */
		return 0;

	/* We can return here if we find only a single empty entry.
	   For simplicity, we just proceed (and make it empty again). */

	/* We're creating one empty entry for combined empty spaces */
	uint32_t addr = cbfs_get_entry_addr(image, entry);
	size_t len = next_addr - addr - cbfs_calculate_file_header_size("");
	DEBUG("join_empty_entry: [0x%x, 0x%x) len=%zu\n", addr, next_addr, len);
	cbfs_create_empty_entry(entry, CBFS_TYPE_NULL, len, "");

	return 0;
}

int cbfs_legacy_walk(struct cbfs_image *image, cbfs_entry_callback callback,
	      void *arg)
{
	int count = 0;
	struct cbfs_file *entry;
	for (entry = cbfs_find_first_entry(image);
	     entry && cbfs_is_valid_entry(image, entry);
	     entry = cbfs_find_next_entry(image, entry)) {
		count ++;
		if (callback(image, entry, arg) != 0)
			break;
	}
	return count;
}

static int cbfs_header_valid(struct cbfs_header *header)
{
	if ((be32toh(header->magic) == CBFS_HEADER_MAGIC) &&
	    ((be32toh(header->version) == CBFS_HEADER_VERSION1) ||
	     (be32toh(header->version) == CBFS_HEADER_VERSION2)) &&
	    (be32toh(header->offset) < be32toh(header->romsize)))
		return 1;
	return 0;
}

struct cbfs_header *cbfs_find_header(char *data, size_t size,
				     uint32_t forced_offset)
{
	size_t offset;
	int found = 0;
	int32_t rel_offset;
	struct cbfs_header *header, *result = NULL;

	if (forced_offset < (size - sizeof(struct cbfs_header))) {
		/* Check if the forced header is valid. */
		header = (struct cbfs_header *)(data + forced_offset);
		if (cbfs_header_valid(header))
			return header;
		return NULL;
	}

	// Try finding relative offset of master header at end of file first.
	rel_offset = *(int32_t *)(data + size - sizeof(int32_t));
	offset = size + rel_offset;
	DEBUG("relative offset: %#zx(-%#zx), offset: %#zx\n",
	      (size_t)rel_offset, (size_t)-rel_offset, offset);

	if (offset >= size - sizeof(*header) ||
	    !cbfs_header_valid((struct cbfs_header *)(data + offset))) {
		// Some use cases append non-CBFS data to the end of the ROM.
		DEBUG("relative offset seems wrong, scanning whole image...\n");
		offset = 0;
	}

	for (; offset + sizeof(*header) < size; offset++) {
		header = (struct cbfs_header *)(data + offset);
		if (!cbfs_header_valid(header))
			continue;
		if (!found++)
			result = header;
	}
	if (found > 1)
		// Top-aligned images usually have a working relative offset
		// field, so this is more likely to happen on bottom-aligned
		// ones (where the first header is the "outermost" one)
		WARN("Multiple (%d) CBFS headers found, using the first one.\n",
		       found);
	return result;
}


struct cbfs_file *cbfs_find_first_entry(struct cbfs_image *image)
{
	assert(image);
	if (image->has_header)
		/* header.offset is relative to start of flash, not
		 * start of region, so use it with the full image.
		 */
		return (struct cbfs_file *)
			(buffer_get_original_backing(&image->buffer) +
			image->header.offset);
	else
		return (struct cbfs_file *)buffer_get(&image->buffer);
}

struct cbfs_file *cbfs_find_next_entry(struct cbfs_image *image,
				       struct cbfs_file *entry)
{
	uint32_t addr = cbfs_get_entry_addr(image, entry);
	int align = image->has_header ? image->header.align : CBFS_ALIGNMENT;
	assert(entry && cbfs_is_valid_entry(image, entry));
	addr += be32toh(entry->offset) + be32toh(entry->len);
	addr = align_up(addr, align);
	return (struct cbfs_file *)(image->buffer.data + addr);
}

uint32_t cbfs_get_entry_addr(struct cbfs_image *image, struct cbfs_file *entry)
{
	assert(image && image->buffer.data && entry);
	return (int32_t)((char *)entry - image->buffer.data);
}

int cbfs_is_valid_cbfs(struct cbfs_image *image)
{
	return buffer_check_magic(&image->buffer, CBFS_FILE_MAGIC,
						strlen(CBFS_FILE_MAGIC));
}

int cbfs_is_legacy_cbfs(struct cbfs_image *image)
{
	return image->has_header;
}

int cbfs_is_valid_entry(struct cbfs_image *image, struct cbfs_file *entry)
{
	uint32_t offset = cbfs_get_entry_addr(image, entry);

	if (offset >= image->buffer.size)
		return 0;

	struct buffer entry_data;
	buffer_clone(&entry_data, &image->buffer);
	buffer_seek(&entry_data, offset);
	return buffer_check_magic(&entry_data, CBFS_FILE_MAGIC,
						strlen(CBFS_FILE_MAGIC));
}

struct cbfs_file *cbfs_create_file_header(int type,
			    size_t len, const char *name)
{
	struct cbfs_file *entry = malloc(CBFS_METADATA_MAX_SIZE);
	memset(entry, CBFS_CONTENT_DEFAULT_VALUE, CBFS_METADATA_MAX_SIZE);
	memcpy(entry->magic, CBFS_FILE_MAGIC, sizeof(entry->magic));
	entry->type = htobe32(type);
	entry->len = htobe32(len);
	entry->attributes_offset = 0;
	entry->offset = htobe32(cbfs_calculate_file_header_size(name));
	memset(entry->filename, 0, be32toh(entry->offset) - sizeof(*entry));
	strcpy(entry->filename, name);
	return entry;
}

int cbfs_create_empty_entry(struct cbfs_file *entry, int type,
			    size_t len, const char *name)
{
	struct cbfs_file *tmp = cbfs_create_file_header(type, len, name);
	memcpy(entry, tmp, be32toh(tmp->offset));
	free(tmp);
	memset(CBFS_SUBHEADER(entry), CBFS_CONTENT_DEFAULT_VALUE, len);
	return 0;
}

struct cbfs_file_attribute *cbfs_file_first_attr(struct cbfs_file *file)
{
	/* attributes_offset should be 0 when there is no attribute, but all
	 * values that point into the cbfs_file header are invalid, too. */
	if (be32toh(file->attributes_offset) <= sizeof(*file))
		return NULL;

	/* There needs to be enough space for the file header and one
	 * attribute header for this to make sense. */
	if (be32toh(file->offset) <=
		sizeof(*file) + sizeof(struct cbfs_file_attribute))
		return NULL;

	return (struct cbfs_file_attribute *)
		(((uint8_t *)file) + be32toh(file->attributes_offset));
}

struct cbfs_file_attribute *cbfs_file_next_attr(struct cbfs_file *file,
	struct cbfs_file_attribute *attr)
{
	/* ex falso sequitur quodlibet */
	if (attr == NULL)
		return NULL;

	/* Is there enough space for another attribute? */
	if ((uint8_t *)attr + be32toh(attr->len) +
		sizeof(struct cbfs_file_attribute) >
		(uint8_t *)file + be32toh(file->offset))
		return NULL;

	struct cbfs_file_attribute *next = (struct cbfs_file_attribute *)
		(((uint8_t *)attr) + be32toh(attr->len));
	/* If any, "unused" attributes must come last. */
	if (be32toh(next->tag) == CBFS_FILE_ATTR_TAG_UNUSED)
		return NULL;
	if (be32toh(next->tag) == CBFS_FILE_ATTR_TAG_UNUSED2)
		return NULL;

	return next;
}

struct cbfs_file_attribute *cbfs_add_file_attr(struct cbfs_file *header,
					       uint32_t tag,
					       uint32_t size)
{
	assert(IS_ALIGNED(size, CBFS_ATTRIBUTE_ALIGN));
	struct cbfs_file_attribute *attr, *next;
	next = cbfs_file_first_attr(header);
	do {
		attr = next;
		next = cbfs_file_next_attr(header, attr);
	} while (next != NULL);
	uint32_t header_size = be32toh(header->offset) + size;
	if (header_size > CBFS_METADATA_MAX_SIZE) {
		DEBUG("exceeding allocated space for cbfs_file headers");
		return NULL;
	}
	/* attr points to the last valid attribute now.
	 * If NULL, we have to create the first one. */
	if (attr == NULL) {
		/* New attributes start where the header ends.
		 * header->offset is later set to accommodate the
		 * additional structure.
		 * No endianness translation necessary here, because both
		 * fields are encoded the same way. */
		header->attributes_offset = header->offset;
		attr = (struct cbfs_file_attribute *)
			(((uint8_t *)header) +
			be32toh(header->attributes_offset));
	} else {
		attr = (struct cbfs_file_attribute *)
			(((uint8_t *)attr) +
			be32toh(attr->len));
	}
	header->offset = htobe32(header_size);
	/* Attributes are expected to be small (much smaller than a flash page)
	   and not really meant to be overwritten in-place. To avoid surprising
	   values in reserved fields of attribute structures, initialize them to
	   0, not 0xff. */
	memset(attr, 0, size);
	attr->tag = htobe32(tag);
	attr->len = htobe32(size);
	return attr;
}

int cbfs_add_file_hash(struct cbfs_file *header, struct buffer *buffer,
	enum vb2_hash_algorithm alg)
{
	if (!vb2_digest_size(alg))
		return -1;

	struct cbfs_file_attr_hash *attr =
		(struct cbfs_file_attr_hash *)cbfs_add_file_attr(header,
			CBFS_FILE_ATTR_TAG_HASH, cbfs_file_attr_hash_size(alg));

	if (attr == NULL)
		return -1;

	if (vb2_hash_calculate(buffer_get(buffer), buffer_size(buffer),
			       alg, &attr->hash) != VB2_SUCCESS)
		return -1;

	return 0;
}

/* Finds a place to hold whole data in same memory page. */
static int is_in_same_page(uint32_t start, uint32_t size, uint32_t page)
{
	if (!page)
		return 1;
	return (start / page) == (start + size - 1) / page;
}

/* Tests if data can fit in a range by given offset:
 *  start ->| metadata_size | offset (+ size) |<- end
 */
static int is_in_range(size_t start, size_t end, size_t metadata_size,
		       size_t offset, size_t size)
{
	return (offset >= start + metadata_size && offset + size <= end);
}

static size_t absolute_align(const struct cbfs_image *image, size_t val,
				size_t align)
{
	const size_t region_offset = buffer_offset(&image->buffer);
	/* To perform alignment on absolute address, take the region offset */
	/* of the image into account.					    */
	return align_up(val + region_offset, align) - region_offset;

}

int32_t cbfs_locate_entry(struct cbfs_image *image, size_t size,
			  size_t page_size, size_t align, size_t metadata_size)
{
	struct cbfs_file *entry;
	size_t need_len;
	size_t addr, addr_next, addr2, addr3, offset;

	/* Default values: allow fitting anywhere in ROM. */
	if (!page_size)
		page_size = image->has_header ? image->header.romsize :
							image->buffer.size;
	if (!align)
		align = 1;

	if (size > page_size)
		ERROR("Input file size (%zd) greater than page size (%zd).\n",
		      size, page_size);

	size_t image_align = image->has_header ? image->header.align :
							CBFS_ALIGNMENT;
	if (page_size % image_align)
		WARN("%s: Page size (%#zx) not aligned with CBFS image (%#zx).\n",
		     __func__, page_size, image_align);

	need_len = metadata_size + size;

	// Merge empty entries to build get max available space.
	cbfs_legacy_walk(image, cbfs_merge_empty_entry, NULL);

	/* Three cases of content location on memory page:
	 * case 1.
	 *          |  PAGE 1  |   PAGE 2  |
	 *          |     <header><content>| Fit. Return start of content.
	 *
	 * case 2.
	 *          |  PAGE 1  |   PAGE 2  |
	 *          | <header><content>    | Fits when we shift content to align
	 *  shift-> |  <header>|<content>  | at starting of PAGE 2.
	 *
	 * case 3. (large content filling whole page)
	 *  | PAGE 1 |  PAGE 2  | PAGE 3 |
	 *  |  <header>< content >       | Can't fit. If we shift content to
	 *  |trial-> <header>< content > | PAGE 2, header can't fit in free
	 *  |  shift->  <header><content> space, so we must use PAGE 3.
	 *
	 * The returned address can be then used as "base-address" (-b) in add-*
	 * commands (will be re-calculated and positioned by cbfs_add_entry_at).
	 * For stage targets, the address is also used to re-link stage before
	 * being added into CBFS.
	 */
	for (entry = cbfs_find_first_entry(image);
	     entry && cbfs_is_valid_entry(image, entry);
	     entry = cbfs_find_next_entry(image, entry)) {

		uint32_t type = be32toh(entry->type);
		if (type != CBFS_TYPE_NULL)
			continue;

		addr = cbfs_get_entry_addr(image, entry);
		addr_next = cbfs_get_entry_addr(image, cbfs_find_next_entry(
				image, entry));
		if (addr_next - addr < need_len)
			continue;

		offset = absolute_align(image, addr + metadata_size, align);
		if (is_in_same_page(offset, size, page_size) &&
		    is_in_range(addr, addr_next, metadata_size, offset, size)) {
			DEBUG("cbfs_locate_entry: FIT (PAGE1).");
			return offset;
		}

		addr2 = align_up(addr, page_size);
		offset = absolute_align(image, addr2, align);
		if (is_in_range(addr, addr_next, metadata_size, offset, size)) {
			DEBUG("cbfs_locate_entry: OVERLAP (PAGE2).");
			return offset;
		}

		/* Assume page_size >= metadata_size so adding one page will
		 * definitely provide the space for header. */
		assert(page_size >= metadata_size);
		addr3 = addr2 + page_size;
		offset = absolute_align(image, addr3, align);
		if (is_in_range(addr, addr_next, metadata_size, offset, size)) {
			DEBUG("cbfs_locate_entry: OVERLAP+ (PAGE3).");
			return offset;
		}
	}
	return -1;
}
