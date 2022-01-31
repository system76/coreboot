/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __CBFSTOOL_COMMON_H
#define __CBFSTOOL_COMMON_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include <commonlib/bsd/cbfs_serialized.h>
#include <commonlib/bsd/sysincludes.h>
#include <commonlib/helpers.h>
#include <console/console.h>

/*
 * There are two address spaces that this tool deals with - SPI flash address space and host
 * address space. This macros checks if the address is greater than 2GiB under the assumption
 * that the low MMIO lives in the top half of the 4G address space of the host.
 */
#define IS_HOST_SPACE_ADDRESS(addr)	((uint32_t)(addr) > 0x80000000)

#define unused __attribute__((unused))

static inline uint32_t align_up(uint32_t value, uint32_t align)
{
	if (value % align)
		value += align - (value % align);
	return value;
}

/* Buffer and file I/O */
struct buffer {
	char *name;
	char *data;
	size_t offset;
	size_t size;
};

static inline void *buffer_get(const struct buffer *b)
{
	return b->data;
}

static inline size_t buffer_size(const struct buffer *b)
{
	return b->size;
}

static inline size_t buffer_offset(const struct buffer *b)
{
	return b->offset;
}

static inline void *buffer_end(const struct buffer *b)
{
	return b->data + b->size;
}

/*
 * Shrink a buffer toward the beginning of its previous space.
 * Afterward, buffer_delete() remains the means of cleaning it up. */
static inline void buffer_set_size(struct buffer *b, size_t size)
{
	b->size = size;
}

/* Initialize a buffer with the given constraints. */
static inline void buffer_init(struct buffer *b, char *name, void *data,
                               size_t size)
{
	b->name = name;
	b->data = data;
	b->size = size;
	b->offset = 0;
}

/* Splice a buffer into another buffer. Note that it's up to the caller to
 * bounds check the offset and size. The resulting buffer is backed by the same
 * storage as the original, so although it is valid to buffer_delete() either
 * one of them, doing so releases both simultaneously. */
static inline void buffer_splice(struct buffer *dest, const struct buffer *src,
                                 size_t offset, size_t size)
{
	dest->name = src->name;
	dest->data = src->data + offset;
	dest->offset = src->offset + offset;
	dest->size = size;
}

/*
 * Shallow copy a buffer. To clean up the resources, buffer_delete()
 * either one, but not both. */
static inline void buffer_clone(struct buffer *dest, const struct buffer *src)
{
	buffer_splice(dest, src, 0, src->size);
}

/*
 * Shrink a buffer toward the end of its previous space.
 * Afterward, buffer_delete() remains the means of cleaning it up. */
static inline void buffer_seek(struct buffer *b, size_t size)
{
	b->offset += size;
	b->size -= size;
	b->data += size;
}

/* Returns whether the buffer begins with the specified magic bytes. */
static inline bool buffer_check_magic(const struct buffer *b, const char *magic,
							size_t magic_len)
{
	assert(magic);
	return b && b->size >= magic_len &&
					memcmp(b->data, magic, magic_len) == 0;
}

/* Returns the start of the underlying buffer, with the offset undone */
static inline void *buffer_get_original_backing(const struct buffer *b)
{
	if (!b)
		return NULL;
	return buffer_get(b) - buffer_offset(b);
}

/* Creates an empty memory buffer with given size.
 * Returns 0 on success, otherwise non-zero. */
int buffer_create(struct buffer *buffer, size_t size, const char *name);

/* Loads a file into memory buffer. Returns 0 on success, otherwise non-zero. */
int buffer_from_file(struct buffer *buffer, const char *filename);

/* Loads a file into memory buffer (with buffer size rounded up to a multiple of
   size_granularity). Returns 0 on success, otherwise non-zero. */
int buffer_from_file_aligned_size(struct buffer *buffer, const char *filename,
				  size_t size_granularity);

/* Writes memory buffer content into file.
 * Returns 0 on success, otherwise non-zero. */
int buffer_write_file(struct buffer *buffer, const char *filename);

/* Destroys a memory buffer. */
void buffer_delete(struct buffer *buffer);

const char *arch_to_string(uint32_t a);
uint32_t string_to_arch(const char *arch_string);

/* Compress in_len bytes from in, storing the result at out, returning the
 * resulting length in out_len.
 * Returns 0 on error,
 *         != 0 otherwise, depending on the compressing function.
 */
typedef int (*comp_func_ptr) (char *in, int in_len, char *out, int *out_len);

/* Decompress in_len bytes from in, storing the result at out, up to out_len
 * bytes.
 * Returns 0 on error,
 *         != 0 otherwise, depending on the decompressing function.
 */
typedef int (*decomp_func_ptr) (char *in, int in_len, char *out, int out_len,
				size_t *actual_size);

comp_func_ptr compression_function(enum cbfs_compression algo);
decomp_func_ptr decompression_function(enum cbfs_compression algo);

uint64_t intfiletype(const char *name);

/* cbfs-mkpayload.c */
int parse_elf_to_payload(const struct buffer *input, struct buffer *output,
			 enum cbfs_compression algo);
int parse_fv_to_payload(const struct buffer *input, struct buffer *output,
			enum cbfs_compression algo);
int parse_fit_to_payload(const struct buffer *input, struct buffer *output,
			 enum cbfs_compression algo);
int parse_bzImage_to_payload(const struct buffer *input,
			     struct buffer *output, const char *initrd,
			     char *cmdline, enum cbfs_compression algo);
int parse_flat_binary_to_payload(const struct buffer *input,
				 struct buffer *output,
				 uint32_t loadaddress,
				 uint32_t entrypoint,
				 enum cbfs_compression algo);
/* cbfs-mkstage.c */
int parse_elf_to_stage(const struct buffer *input, struct buffer *output,
		       const char *ignore_section,
		       struct cbfs_file_attr_stageheader *stageheader);
/* location is TOP aligned. */
int parse_elf_to_xip_stage(const struct buffer *input, struct buffer *output,
			   uint32_t location, const char *ignore_section,
			   struct cbfs_file_attr_stageheader *stageheader);

void print_supported_architectures(void);
void print_supported_filetypes(void);

/* lzma/lzma.c */
int do_lzma_compress(char *in, int in_len, char *out, int *out_len);
int do_lzma_uncompress(char *dst, int dst_len, char *src, int src_len,
			size_t *actual_size);

/* xdr.c */
struct xdr {
	uint8_t (*get8)(struct buffer *input);
	uint16_t (*get16)(struct buffer *input);
	uint32_t (*get32)(struct buffer *input);
	uint64_t (*get64)(struct buffer *input);
	void (*put8)(struct buffer *input, uint8_t val);
	void (*put16)(struct buffer *input, uint16_t val);
	void (*put32)(struct buffer *input, uint32_t val);
	void (*put64)(struct buffer *input, uint64_t val);
};

extern struct xdr xdr_le, xdr_be;
size_t bgets(struct buffer *input, void *output, size_t len);
size_t bputs(struct buffer *b, const void *data, size_t len);

/* Returns a 0-terminated string containing a hex representation of
 * len bytes starting at data.
 * The string is malloc'd and it's the caller's responsibility to free
 * the memory.
 * On error, bintohex returns NULL.
 */
char *bintohex(uint8_t *data, size_t len);
#endif
