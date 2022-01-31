/* SPDX-License-Identifier: GPL-2.0-only */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <libgen.h>
#include <assert.h>
#include <regex.h>
#include <commonlib/bsd/cbmem_id.h>
#include <commonlib/timestamp_serialized.h>
#include <commonlib/tcpa_log_serialized.h>
#include <commonlib/coreboot_tables.h>

#ifdef __OpenBSD__
#include <sys/param.h>
#include <sys/sysctl.h>
#endif

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

/* Return < 0 on error, 0 on success. */
static int parse_cbtable(u64 address, size_t table_size);

struct mapping {
	void *virt;
	size_t offset;
	size_t virt_size;
	unsigned long long phys;
	size_t size;
};

#define CBMEM_VERSION "1.1"

/* verbose output? */
static int verbose = 0;
#define debug(x...) if(verbose) printf(x)

/* File handle used to access /dev/mem */
static int mem_fd;
static struct mapping lbtable_mapping;

static void die(const char *msg)
{
	if (msg)
		fputs(msg, stderr);
	exit(1);
}

static unsigned long long system_page_size(void)
{
	static unsigned long long page_size;

	if (!page_size)
		page_size = getpagesize();

	return page_size;
}

static inline size_t size_to_mib(size_t sz)
{
	return sz >> 20;
}

/* Return mapping of physical address requested. */
static const void *mapping_virt(const struct mapping *mapping)
{
	const char *v = mapping->virt;

	if (v == NULL)
		return NULL;

	return v + mapping->offset;
}

/* Returns virtual address on success, NULL on error. mapping is filled in. */
static const void *map_memory(struct mapping *mapping, unsigned long long phys,
				size_t sz)
{
	void *v;
	unsigned long long page_size;

	page_size = system_page_size();

	mapping->virt = NULL;
	mapping->offset = phys % page_size;
	mapping->virt_size = sz + mapping->offset;
	mapping->size = sz;
	mapping->phys = phys;

	if (size_to_mib(mapping->virt_size) == 0) {
		debug("Mapping %zuB of physical memory at 0x%llx (requested 0x%llx).\n",
			mapping->virt_size, phys - mapping->offset, phys);
	} else {
		debug("Mapping %zuMB of physical memory at 0x%llx (requested 0x%llx).\n",
			size_to_mib(mapping->virt_size), phys - mapping->offset,
			phys);
	}

	v = mmap(NULL, mapping->virt_size, PROT_READ, MAP_SHARED, mem_fd,
			phys - mapping->offset);

	if (v == MAP_FAILED) {
		debug("Mapping failed %zuB of physical memory at 0x%llx.\n",
			mapping->virt_size, phys - mapping->offset);
		return NULL;
	}

	mapping->virt = v;

	if (mapping->offset != 0)
		debug("  ... padding virtual address with 0x%zx bytes.\n",
			mapping->offset);

	return mapping_virt(mapping);
}

/* Returns 0 on success, < 0 on error. mapping is cleared if successful. */
static int unmap_memory(struct mapping *mapping)
{
	if (mapping->virt == NULL)
		return -1;

	munmap(mapping->virt, mapping->virt_size);
	mapping->virt = NULL;
	mapping->offset = 0;
	mapping->virt_size = 0;

	return 0;
}

/* Return size of physical address mapping requested. */
static size_t mapping_size(const struct mapping *mapping)
{
	if (mapping->virt == NULL)
		return 0;

	return mapping->size;
}

/*
 * Some architectures map /dev/mem memory in a way that doesn't support
 * unaligned accesses. Most normal libc memcpy()s aren't safe to use in this
 * case, so build our own which makes sure to never do unaligned accesses on
 * *src (*dest is fine since we never map /dev/mem for writing).
 */
static void *aligned_memcpy(void *dest, const void *src, size_t n)
{
	u8 *d = dest;
	const volatile u8 *s = src;	/* volatile to prevent optimization */

	while ((uintptr_t)s & (sizeof(size_t) - 1)) {
		if (n-- == 0)
			return dest;
		*d++ = *s++;
	}

	while (n >= sizeof(size_t)) {
		*(size_t *)d = *(const volatile size_t *)s;
		d += sizeof(size_t);
		s += sizeof(size_t);
		n -= sizeof(size_t);
	}

	while (n-- > 0)
		*d++ = *s++;

	return dest;
}

/*
 * calculate ip checksum (16 bit quantities) on a passed in buffer. In case
 * the buffer length is odd last byte is excluded from the calculation
 */
static u16 ipchcksum(const void *addr, unsigned size)
{
	const u16 *p = addr;
	unsigned i, n = size / 2; /* don't expect odd sized blocks */
	u32 sum = 0;

	for (i = 0; i < n; i++)
		sum += p[i];

	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	sum = ~sum & 0xffff;
	return (u16) sum;
}

/* Find the first cbmem entry filling in the details. */
static int find_cbmem_entry(uint32_t id, uint64_t *addr, size_t *size)
{
	const uint8_t *table;
	size_t offset;
	int ret = -1;

	table = mapping_virt(&lbtable_mapping);

	if (table == NULL)
		return -1;

	offset = 0;

	while (offset < mapping_size(&lbtable_mapping)) {
		const struct lb_record *lbr;
		const struct lb_cbmem_entry *lbe;

		lbr = (const void *)(table + offset);
		offset += lbr->size;

		if (lbr->tag != LB_TAG_CBMEM_ENTRY)
			continue;

		lbe = (const void *)lbr;
		if (lbe->id != id)
			continue;

		*addr = lbe->address;
		*size = lbe->entry_size;
		ret = 0;
		break;
	}

	return ret;
}

/*
 * Try finding the timestamp table and coreboot cbmem console starting from the
 * passed in memory offset.  Could be called recursively in case a forwarding
 * entry is found.
 *
 * Returns pointer to a memory buffer containing the timestamp table or zero if
 * none found.
 */

static struct lb_cbmem_ref timestamps;
static struct lb_cbmem_ref console;
static struct lb_cbmem_ref tcpa_log;
static struct lb_memory_range cbmem;

/* This is a work-around for a nasty problem introduced by initially having
 * pointer sized entries in the lb_cbmem_ref structures. This caused problems
 * on 64bit x86 systems because coreboot is 32bit on those systems.
 * When the problem was found, it was corrected, but there are a lot of
 * systems out there with a firmware that does not produce the right
 * lb_cbmem_ref structure. Hence we try to autocorrect this issue here.
 */
static struct lb_cbmem_ref parse_cbmem_ref(const struct lb_cbmem_ref *cbmem_ref)
{
	struct lb_cbmem_ref ret;

	aligned_memcpy(&ret, cbmem_ref, sizeof(ret));

	if (cbmem_ref->size < sizeof(*cbmem_ref))
		ret.cbmem_addr = (uint32_t)ret.cbmem_addr;

	debug("      cbmem_addr = %" PRIx64 "\n", ret.cbmem_addr);

	return ret;
}

static void parse_memory_tags(const struct lb_memory *mem)
{
	int num_entries;
	int i;

	/* Peel off the header size and calculate the number of entries. */
	num_entries = (mem->size - sizeof(*mem)) / sizeof(mem->map[0]);

	for (i = 0; i < num_entries; i++) {
		if (mem->map[i].type != LB_MEM_TABLE)
			continue;
		debug("      LB_MEM_TABLE found.\n");
		/* The last one found is CBMEM */
		aligned_memcpy(&cbmem, &mem->map[i], sizeof(cbmem));
	}
}

/* Return < 0 on error, 0 on success, 1 if forwarding table entry found. */
static int parse_cbtable_entries(const struct mapping *table_mapping)
{
	size_t i;
	const struct lb_record *lbr_p;
	size_t table_size = mapping_size(table_mapping);
	const void *lbtable = mapping_virt(table_mapping);
	int forwarding_table_found = 0;

	for (i = 0; i < table_size; i += lbr_p->size) {
		lbr_p = lbtable + i;
		debug("  coreboot table entry 0x%02x\n", lbr_p->tag);
		switch (lbr_p->tag) {
		case LB_TAG_MEMORY:
			debug("    Found memory map.\n");
			parse_memory_tags(lbtable + i);
			continue;
		case LB_TAG_TIMESTAMPS: {
			debug("    Found timestamp table.\n");
			timestamps =
			    parse_cbmem_ref((struct lb_cbmem_ref *)lbr_p);
			continue;
		}
		case LB_TAG_CBMEM_CONSOLE: {
			debug("    Found cbmem console.\n");
			console = parse_cbmem_ref((struct lb_cbmem_ref *)lbr_p);
			continue;
		}
		case LB_TAG_TCPA_LOG: {
			debug("    Found tcpa log table.\n");
			tcpa_log =
			    parse_cbmem_ref((struct lb_cbmem_ref *)lbr_p);
			continue;
		}
		case LB_TAG_FORWARD: {
			int ret;
			/*
			 * This is a forwarding entry - repeat the
			 * search at the new address.
			 */
			struct lb_forward lbf_p =
			    *(const struct lb_forward *)lbr_p;
			debug("    Found forwarding entry.\n");
			ret = parse_cbtable(lbf_p.forward, 0);

			/* Assume the forwarding entry is valid. If this fails
			 * then there's a total failure. */
			if (ret < 0)
				return -1;
			forwarding_table_found = 1;
		}
		default:
			break;
		}
	}

	return forwarding_table_found;
}

/* Return < 0 on error, 0 on success. */
static int parse_cbtable(u64 address, size_t table_size)
{
	const void *buf;
	struct mapping header_mapping;
	size_t req_size;
	size_t i;

	req_size = table_size;
	/* Default to 4 KiB search space. */
	if (req_size == 0)
		req_size = 4 * 1024;

	debug("Looking for coreboot table at %" PRIx64 " %zd bytes.\n",
		address, req_size);

	buf = map_memory(&header_mapping, address, req_size);

	if (!buf)
		return -1;

	/* look at every 16 bytes */
	for (i = 0; i <= req_size - sizeof(struct lb_header); i += 16) {
		int ret;
		const struct lb_header *lbh;
		struct mapping table_mapping;

		lbh = buf + i;
		if (memcmp(lbh->signature, "LBIO", sizeof(lbh->signature)) ||
		    !lbh->header_bytes ||
		    ipchcksum(lbh, sizeof(*lbh))) {
			continue;
		}

		/* Map in the whole table to parse. */
		if (!map_memory(&table_mapping, address + i + lbh->header_bytes,
				 lbh->table_bytes)) {
			debug("Couldn't map in table\n");
			continue;
		}

		if (ipchcksum(mapping_virt(&table_mapping), lbh->table_bytes) !=
		    lbh->table_checksum) {
			debug("Signature found, but wrong checksum.\n");
			unmap_memory(&table_mapping);
			continue;
		}

		debug("Found!\n");

		ret = parse_cbtable_entries(&table_mapping);

		/* Table parsing failed. */
		if (ret < 0) {
			unmap_memory(&table_mapping);
			continue;
		}

		/* Succeeded in parsing the table. Header not needed anymore. */
		unmap_memory(&header_mapping);

		/*
		 * Table parsing succeeded. If forwarding table not found update
		 * coreboot table mapping for future use.
		 */
		if (ret == 0)
			lbtable_mapping = table_mapping;
		else
			unmap_memory(&table_mapping);

		return 0;
	}

	unmap_memory(&header_mapping);

	return -1;
}

#if defined(linux) && (defined(__i386__) || defined(__x86_64__))
/*
 * read CPU frequency from a sysfs file, return an frequency in Megahertz as
 * an int or exit on any error.
 */
static unsigned long arch_tick_frequency(void)
{
	FILE *cpuf;
	char freqs[100];
	int  size;
	char *endp;
	u64 rv;

	const char* freq_file =
		"/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq";

	cpuf = fopen(freq_file, "r");
	if (!cpuf) {
		fprintf(stderr, "Could not open %s: %s\n",
			freq_file, strerror(errno));
		exit(1);
	}

	memset(freqs, 0, sizeof(freqs));
	size = fread(freqs, 1, sizeof(freqs), cpuf);
	if (!size || (size == sizeof(freqs))) {
		fprintf(stderr, "Wrong number of bytes(%d) read from %s\n",
			size, freq_file);
		exit(1);
	}
	fclose(cpuf);
	rv = strtoull(freqs, &endp, 10);

	if (*endp == '\0' || *endp == '\n')
	/* cpuinfo_max_freq is in kHz. Convert it to MHz. */
		return rv / 1000;
	fprintf(stderr, "Wrong formatted value ^%s^ read from %s\n",
		freqs, freq_file);
	exit(1);
}
#elif defined(__OpenBSD__) && (defined(__i386__) || defined(__x86_64__))
static unsigned long arch_tick_frequency(void)
{
	int mib[2] = { CTL_HW, HW_CPUSPEED };
	static int value = 0;
	size_t value_len = sizeof(value);

	/* Return 1 MHz when sysctl fails. */
	if ((value == 0) && (sysctl(mib, 2, &value, &value_len, NULL, 0) == -1))
		return 1;

	return value;
}
#else
static unsigned long arch_tick_frequency(void)
{
	/* 1 MHz = 1us. */
	return 1;
}
#endif

static unsigned long tick_freq_mhz;

static void timestamp_set_tick_freq(unsigned long table_tick_freq_mhz)
{
	tick_freq_mhz = table_tick_freq_mhz;

	/* Honor table frequency if present. */
	if (!tick_freq_mhz)
		tick_freq_mhz = arch_tick_frequency();

	if (!tick_freq_mhz) {
		fprintf(stderr, "Cannot determine timestamp tick frequency.\n");
		exit(1);
	}

	debug("Timestamp tick frequency: %ld MHz\n", tick_freq_mhz);
}

static u64 arch_convert_raw_ts_entry(u64 ts)
{
	return ts / tick_freq_mhz;
}

/*
 * Print an integer in 'normalized' form - with commas separating every three
 * decimal orders.
 */
static void print_norm(u64 v)
{
	if (v >= 1000) {
		/* print the higher order sections first */
		print_norm(v / 1000);
		printf(",%3.3u", (u32)(v % 1000));
	} else {
		printf("%u", (u32)(v % 1000));
	}
}

static const char *timestamp_name(uint32_t id)
{
	for (size_t i = 0; i < ARRAY_SIZE(timestamp_ids); i++) {
		if (timestamp_ids[i].id == id)
			return timestamp_ids[i].name;
	}
	return "<unknown>";
}

static uint64_t timestamp_print_parseable_entry(uint32_t id, uint64_t stamp,
						uint64_t prev_stamp)
{
	const char *name;
	uint64_t step_time;

	name = timestamp_name(id);

	step_time = arch_convert_raw_ts_entry(stamp - prev_stamp);

	/* ID<tab>absolute time<tab>relative time<tab>description */
	printf("%d\t", id);
	printf("%llu\t", (long long)arch_convert_raw_ts_entry(stamp));
	printf("%llu\t", (long long)step_time);
	printf("%s\n", name);

	return step_time;
}

static uint64_t timestamp_print_entry(uint32_t id, uint64_t stamp, uint64_t prev_stamp)
{
	const char *name;
	uint64_t step_time;

	name = timestamp_name(id);

	printf("%4d:", id);
	printf("%-50s", name);
	print_norm(arch_convert_raw_ts_entry(stamp));
	step_time = arch_convert_raw_ts_entry(stamp - prev_stamp);
	if (prev_stamp) {
		printf(" (");
		print_norm(step_time);
		printf(")");
	}
	printf("\n");

	return step_time;
}

static int compare_timestamp_entries(const void *a, const void *b)
{
	const struct timestamp_entry *tse_a = (struct timestamp_entry *)a;
	const struct timestamp_entry *tse_b = (struct timestamp_entry *)b;

	if (tse_a->entry_stamp > tse_b->entry_stamp)
		return 1;
	else if (tse_a->entry_stamp < tse_b->entry_stamp)
		return -1;

	return 0;
}

/* dump the timestamp table */
static void dump_timestamps(int mach_readable)
{
	const struct timestamp_table *tst_p;
	struct timestamp_table *sorted_tst_p;
	size_t size;
	uint64_t prev_stamp;
	uint64_t total_time;
	struct mapping timestamp_mapping;

	if (timestamps.tag != LB_TAG_TIMESTAMPS) {
		fprintf(stderr, "No timestamps found in coreboot table.\n");
		return;
	}

	size = sizeof(*tst_p);
	tst_p = map_memory(&timestamp_mapping, timestamps.cbmem_addr, size);
	if (!tst_p)
		die("Unable to map timestamp header\n");

	timestamp_set_tick_freq(tst_p->tick_freq_mhz);

	if (!mach_readable)
		printf("%d entries total:\n\n", tst_p->num_entries);
	size += tst_p->num_entries * sizeof(tst_p->entries[0]);

	unmap_memory(&timestamp_mapping);

	tst_p = map_memory(&timestamp_mapping, timestamps.cbmem_addr, size);
	if (!tst_p)
		die("Unable to map full timestamp table\n");

	sorted_tst_p = malloc(size + sizeof(struct timestamp_entry));
	if (!sorted_tst_p)
		die("Failed to allocate memory");
	aligned_memcpy(sorted_tst_p, tst_p, size);

	/*
	 * Insert a timestamp to represent the base time (start of coreboot),
	 * in case we have to rebase for negative timestamps below.
	 */
	sorted_tst_p->entries[tst_p->num_entries].entry_id = 0;
	sorted_tst_p->entries[tst_p->num_entries].entry_stamp = 0;
	sorted_tst_p->num_entries += 1;

	qsort(&sorted_tst_p->entries[0], sorted_tst_p->num_entries,
	      sizeof(struct timestamp_entry), compare_timestamp_entries);

	/*
	 * If there are negative timestamp entries, rebase all of the
	 * timestamps to the lowest one in the list.
	 */
	if (sorted_tst_p->entries[0].entry_stamp < 0)
		sorted_tst_p->base_time = -sorted_tst_p->entries[0].entry_stamp;
	prev_stamp = 0;

	total_time = 0;
	for (uint32_t i = 0; i < sorted_tst_p->num_entries; i++) {
		uint64_t stamp;
		const struct timestamp_entry *tse = &sorted_tst_p->entries[i];

		/* Make all timestamps absolute. */
		stamp = tse->entry_stamp + sorted_tst_p->base_time;
		if (mach_readable)
			total_time +=
				timestamp_print_parseable_entry(tse->entry_id,
							stamp, prev_stamp);
		else
			total_time += timestamp_print_entry(tse->entry_id,
							stamp, prev_stamp);
		prev_stamp = stamp;
	}

	if (!mach_readable) {
		printf("\nTotal Time: ");
		print_norm(total_time);
		printf("\n");
	}

	unmap_memory(&timestamp_mapping);
	free(sorted_tst_p);
}

/* dump the tcpa log table */
static void dump_tcpa_log(void)
{
	const struct tcpa_table *tclt_p;
	size_t size;
	struct mapping tcpa_mapping;

	if (tcpa_log.tag != LB_TAG_TCPA_LOG) {
		fprintf(stderr, "No tcpa log found in coreboot table.\n");
		return;
	}

	size = sizeof(*tclt_p);
	tclt_p = map_memory(&tcpa_mapping, tcpa_log.cbmem_addr, size);
	if (!tclt_p)
		die("Unable to map tcpa log header\n");

	size += tclt_p->num_entries * sizeof(tclt_p->entries[0]);

	unmap_memory(&tcpa_mapping);

	tclt_p = map_memory(&tcpa_mapping, tcpa_log.cbmem_addr, size);
	if (!tclt_p)
		die("Unable to map full tcpa log table\n");

	printf("coreboot TCPA log:\n\n");

	for (uint16_t i = 0; i < tclt_p->num_entries; i++) {
		const struct tcpa_entry *tce = &tclt_p->entries[i];

		printf(" PCR-%u ", tce->pcr);

		for (uint32_t j = 0; j < tce->digest_length; j++)
			printf("%02x", tce->digest[j]);

		printf(" %s [%s]\n", tce->digest_type, tce->name);
	}

	unmap_memory(&tcpa_mapping);
}

struct cbmem_console {
	u32 size;
	u32 cursor;
	u8  body[0];
}  __attribute__ ((__packed__));

#define CBMC_CURSOR_MASK ((1 << 28) - 1)
#define CBMC_OVERFLOW (1 << 31)

enum console_print_type {
	CONSOLE_PRINT_FULL = 0,
	CONSOLE_PRINT_LAST,
	CONSOLE_PRINT_PREVIOUS,
};

/* dump the cbmem console */
static void dump_console(enum console_print_type type)
{
	const struct cbmem_console *console_p;
	char *console_c;
	size_t size, cursor, previous;
	struct mapping console_mapping;

	if (console.tag != LB_TAG_CBMEM_CONSOLE) {
		fprintf(stderr, "No console found in coreboot table.\n");
		return;
	}

	size = sizeof(*console_p);
	console_p = map_memory(&console_mapping, console.cbmem_addr, size);
	if (!console_p)
		die("Unable to map console object.\n");

	cursor = console_p->cursor & CBMC_CURSOR_MASK;
	if (!(console_p->cursor & CBMC_OVERFLOW) && cursor < console_p->size)
		size = cursor;
	else
		size = console_p->size;
	unmap_memory(&console_mapping);

	console_c = malloc(size + 1);
	if (!console_c) {
		fprintf(stderr, "Not enough memory for console.\n");
		exit(1);
	}
	console_c[size] = '\0';

	console_p = map_memory(&console_mapping, console.cbmem_addr,
		size + sizeof(*console_p));

	if (!console_p)
		die("Unable to map full console object.\n");

	if (console_p->cursor & CBMC_OVERFLOW) {
		if (cursor >= size) {
			printf("cbmem: ERROR: CBMEM console struct is illegal, "
			       "output may be corrupt or out of order!\n\n");
			cursor = 0;
		}
		aligned_memcpy(console_c, console_p->body + cursor,
			       size - cursor);
		aligned_memcpy(console_c + size - cursor,
			       console_p->body, cursor);
	} else {
		aligned_memcpy(console_c, console_p->body, size);
	}

	/* Slight memory corruption may occur between reboots and give us a few
	   unprintable characters like '\0'. Replace them with '?' on output. */
	for (cursor = 0; cursor < size; cursor++)
		if (!isprint(console_c[cursor]) && !isspace(console_c[cursor]))
			console_c[cursor] = '?';

	/* We detect the reboot cutoff by looking for a bootblock, romstage or
	   ramstage banner, in that order (to account for platforms without
	   CONFIG_BOOTBLOCK_CONSOLE and/or CONFIG_EARLY_CONSOLE). Once we find
	   a banner, store the last two matches for that stage and stop. */
	cursor = previous = 0;
	if (type != CONSOLE_PRINT_FULL) {
#define BANNER_REGEX(stage) \
		"\n\ncoreboot-[^\n]* " stage " starting.*\\.\\.\\.\n"
#define OVERFLOW_REGEX(stage) "\n\\*\\*\\* Pre-CBMEM " stage " console overflow"
		const char *regex[] = { BANNER_REGEX("verstage-before-bootblock"),
					BANNER_REGEX("bootblock"),
					BANNER_REGEX("verstage"),
					OVERFLOW_REGEX("romstage"),
					BANNER_REGEX("romstage"),
					OVERFLOW_REGEX("ramstage"),
					BANNER_REGEX("ramstage") };

		for (size_t i = 0; !cursor && i < ARRAY_SIZE(regex); i++) {
			regex_t re;
			regmatch_t match;
			assert(!regcomp(&re, regex[i], 0));

			/* Keep looking for matches so we find the last one. */
			while (!regexec(&re, console_c + cursor, 1, &match, 0)) {
				previous = cursor;
				cursor += match.rm_so + 1;
			}
			regfree(&re);
		}
	}

	if (type == CONSOLE_PRINT_PREVIOUS) {
		console_c[cursor] = '\0';
		cursor = previous;
	}

	puts(console_c + cursor);
	free(console_c);
	unmap_memory(&console_mapping);
}

static void hexdump(unsigned long memory, int length)
{
	int i;
	const uint8_t *m;
	int all_zero = 0;
	struct mapping hexdump_mapping;

	m = map_memory(&hexdump_mapping, memory, length);
	if (!m)
		die("Unable to map hexdump memory.\n");

	for (i = 0; i < length; i += 16) {
		int j;

		all_zero++;
		for (j = 0; j < 16; j++) {
			if(m[i+j] != 0) {
				all_zero = 0;
				break;
			}
		}

		if (all_zero < 2) {
			printf("%08lx:", memory + i);
			for (j = 0; j < 16; j++)
				printf(" %02x", m[i+j]);
			printf("  ");
			for (j = 0; j < 16; j++)
				printf("%c", isprint(m[i+j]) ? m[i+j] : '.');
			printf("\n");
		} else if (all_zero == 2) {
			printf("...\n");
		}
	}

	unmap_memory(&hexdump_mapping);
}

static void dump_cbmem_hex(void)
{
	if (cbmem.type != LB_MEM_TABLE) {
		fprintf(stderr, "No coreboot CBMEM area found!\n");
		return;
	}

	hexdump(unpack_lb64(cbmem.start), unpack_lb64(cbmem.size));
}

static void rawdump(uint64_t base, uint64_t size)
{
	const uint8_t *m;
	struct mapping dump_mapping;

	m = map_memory(&dump_mapping, base, size);
	if (!m)
		die("Unable to map rawdump memory\n");

	for (uint64_t i = 0 ; i < size; i++)
		printf("%c", m[i]);

	unmap_memory(&dump_mapping);
}

static void dump_cbmem_raw(unsigned int id)
{
	const uint8_t *table;
	size_t offset;
	uint64_t base = 0;
	uint64_t size = 0;

	table = mapping_virt(&lbtable_mapping);

	if (table == NULL)
		return;

	offset = 0;

	while (offset < mapping_size(&lbtable_mapping)) {
		const struct lb_record *lbr;
		const struct lb_cbmem_entry *lbe;

		lbr = (const void *)(table + offset);
		offset += lbr->size;

		if (lbr->tag != LB_TAG_CBMEM_ENTRY)
			continue;

		lbe = (const void *)lbr;
		if (lbe->id == id) {
			debug("found id for raw dump %0x", lbe->id);
			base = lbe->address;
			size = lbe->entry_size;
			break;
		}
	}

	if (!base)
		fprintf(stderr, "id %0x not found in cbtable\n", id);
	else
		rawdump(base, size);
}

struct cbmem_id_to_name {
	uint32_t id;
	const char *name;
};
static const struct cbmem_id_to_name cbmem_ids[] = { CBMEM_ID_TO_NAME_TABLE };

#define MAX_STAGEx 10
static void cbmem_print_entry(int n, uint32_t id, uint64_t base, uint64_t size)
{
	const char *name;
	char stage_x[20];

	name = NULL;
	for (size_t i = 0; i < ARRAY_SIZE(cbmem_ids); i++) {
		if (cbmem_ids[i].id == id) {
			name = cbmem_ids[i].name;
			break;
		}
		if (id >= CBMEM_ID_STAGEx_META &&
			id < CBMEM_ID_STAGEx_META + MAX_STAGEx) {
			snprintf(stage_x, sizeof(stage_x), "STAGE%d META",
				(id - CBMEM_ID_STAGEx_META));
			name = stage_x;
		}
		if (id >= CBMEM_ID_STAGEx_CACHE &&
			id < CBMEM_ID_STAGEx_CACHE + MAX_STAGEx) {
			snprintf(stage_x, sizeof(stage_x), "STAGE%d $  ",
				(id - CBMEM_ID_STAGEx_CACHE));
			name = stage_x;
		}
	}

	printf("%2d. ", n);
	if (name == NULL)
		printf("\t\t%08x", id);
	else
		printf("%s\t%08x", name, id);
	printf("  %08" PRIx64 " ", base);
	printf("  %08" PRIx64 "\n", size);
}

static void dump_cbmem_toc(void)
{
	int i;
	const uint8_t *table;
	size_t offset;

	table = mapping_virt(&lbtable_mapping);

	if (table == NULL)
		return;

	printf("CBMEM table of contents:\n");
	printf("    NAME          ID           START      LENGTH\n");

	i = 0;
	offset = 0;

	while (offset < mapping_size(&lbtable_mapping)) {
		const struct lb_record *lbr;
		const struct lb_cbmem_entry *lbe;

		lbr = (const void *)(table + offset);
		offset += lbr->size;

		if (lbr->tag != LB_TAG_CBMEM_ENTRY)
			continue;

		lbe = (const void *)lbr;
		cbmem_print_entry(i, lbe->id, lbe->address, lbe->entry_size);
		i++;
	}
}

#define COVERAGE_MAGIC 0x584d4153
struct file {
	uint32_t magic;
	uint32_t next;
	uint32_t filename;
	uint32_t data;
	int offset;
	int len;
};

static int mkpath(char *path, mode_t mode)
{
	assert (path && *path);
	char *p;
	for (p = strchr(path+1, '/'); p; p = strchr(p + 1, '/')) {
		*p = '\0';
		if (mkdir(path, mode) == -1) {
			if (errno != EEXIST) {
				*p = '/';
				return -1;
			}
		}
		*p = '/';
	}
	return 0;
}

static void dump_coverage(void)
{
	uint64_t start;
	size_t size;
	const void *coverage;
	struct mapping coverage_mapping;
	unsigned long phys_offset;
#define phys_to_virt(x) ((void *)(unsigned long)(x) + phys_offset)

	if (find_cbmem_entry(CBMEM_ID_COVERAGE, &start, &size)) {
		fprintf(stderr, "No coverage information found\n");
		return;
	}

	/* Map coverage area */
	coverage = map_memory(&coverage_mapping, start, size);
	if (!coverage)
		die("Unable to map coverage area.\n");
	phys_offset = (unsigned long)coverage - (unsigned long)start;

	printf("Dumping coverage data...\n");

	struct file *file = (struct file *)coverage;
	while (file && file->magic == COVERAGE_MAGIC) {
		FILE *f;
		char *filename;

		debug(" -> %s\n", (char *)phys_to_virt(file->filename));
		filename = strdup((char *)phys_to_virt(file->filename));
		if (mkpath(filename, 0755) == -1) {
			perror("Directory for coverage data could "
				"not be created");
			exit(1);
		}
		f = fopen(filename, "wb");
		if (!f) {
			printf("Could not open %s: %s\n",
				filename, strerror(errno));
			exit(1);
		}
		if (fwrite((void *)phys_to_virt(file->data),
						file->len, 1, f) != 1) {
			printf("Could not write to %s: %s\n",
				filename, strerror(errno));
			exit(1);
		}
		fclose(f);
		free(filename);

		if (file->next)
			file = (struct file *)phys_to_virt(file->next);
		else
			file = NULL;
	}
	unmap_memory(&coverage_mapping);
}

static void print_version(void)
{
	printf("cbmem v%s -- ", CBMEM_VERSION);
	printf("Copyright (C) 2012 The ChromiumOS Authors.  All rights reserved.\n\n");
	printf(
    "This program is free software: you can redistribute it and/or modify\n"
    "it under the terms of the GNU General Public License as published by\n"
    "the Free Software Foundation, version 2 of the License.\n\n"
    "This program is distributed in the hope that it will be useful,\n"
    "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
    "GNU General Public License for more details.\n\n");
}

static void print_usage(const char *name, int exit_code)
{
	printf("usage: %s [-cCltTLxVvh?]\n", name);
	printf("\n"
	     "   -c | --console:                   print cbmem console\n"
	     "   -1 | --oneboot:                   print cbmem console for last boot only\n"
	     "   -2 | --2ndtolast:                 print cbmem console for the boot that came before the last one only\n"
	     "   -C | --coverage:                  dump coverage information\n"
	     "   -l | --list:                      print cbmem table of contents\n"
	     "   -x | --hexdump:                   print hexdump of cbmem area\n"
	     "   -r | --rawdump ID:                print rawdump of specific ID (in hex) of cbtable\n"
	     "   -t | --timestamps:                print timestamp information\n"
	     "   -T | --parseable-timestamps:      print parseable timestamps\n"
	     "   -L | --tcpa-log                   print TCPA log\n"
	     "   -V | --verbose:                   verbose (debugging) output\n"
	     "   -v | --version:                   print the version\n"
	     "   -h | --help:                      print this help\n"
	     "\n");
	exit(exit_code);
}

#if defined(__arm__) || defined(__aarch64__)
static void dt_update_cells(const char *name, int *addr_cells_ptr,
			    int *size_cells_ptr)
{
	if (*addr_cells_ptr >= 0 && *size_cells_ptr >= 0)
		return;

	int buffer;
	size_t nlen = strlen(name);
	char *prop = alloca(nlen + sizeof("/#address-cells"));
	strcpy(prop, name);

	if (*addr_cells_ptr < 0) {
		strcpy(prop + nlen, "/#address-cells");
		int fd = open(prop, O_RDONLY);
		if (fd < 0 && errno != ENOENT) {
			perror(prop);
		} else if (fd >= 0) {
			if (read(fd, &buffer, sizeof(int)) < 0)
				perror(prop);
			else
				*addr_cells_ptr = ntohl(buffer);
			close(fd);
		}
	}

	if (*size_cells_ptr < 0) {
		strcpy(prop + nlen, "/#size-cells");
		int fd = open(prop, O_RDONLY);
		if (fd < 0 && errno != ENOENT) {
			perror(prop);
		} else if (fd >= 0) {
			if (read(fd, &buffer, sizeof(int)) < 0)
				perror(prop);
			else
				*size_cells_ptr = ntohl(buffer);
			close(fd);
		}
	}
}

static char *dt_find_compat(const char *parent, const char *compat,
			    int *addr_cells_ptr, int *size_cells_ptr)
{
	char *ret = NULL;
	struct dirent *entry;
	DIR *dir;

	if (!(dir = opendir(parent))) {
		perror(parent);
		return NULL;
	}

	/* Loop through all files in the directory (DT node). */
	while ((entry = readdir(dir))) {
		/* We only care about compatible props or subnodes. */
		if (entry->d_name[0] == '.' || !((entry->d_type & DT_DIR) ||
		    !strcmp(entry->d_name, "compatible")))
			continue;

		/* Assemble the file name (on the stack, for speed). */
		size_t plen = strlen(parent);
		char *name = alloca(plen + strlen(entry->d_name) + 2);

		strcpy(name, parent);
		name[plen] = '/';
		strcpy(name + plen + 1, entry->d_name);

		/* If it's a subnode, recurse. */
		if (entry->d_type & DT_DIR) {
			ret = dt_find_compat(name, compat, addr_cells_ptr,
					     size_cells_ptr);

			/* There is only one matching node to find, abort. */
			if (ret) {
				/* Gather cells values on the way up. */
				dt_update_cells(parent, addr_cells_ptr,
						size_cells_ptr);
				break;
			}
			continue;
		}

		/* If it's a compatible string, see if it's the right one. */
		int fd = open(name, O_RDONLY);
		int clen = strlen(compat);
		char *buffer = alloca(clen + 1);

		if (fd < 0) {
			perror(name);
			continue;
		}

		if (read(fd, buffer, clen + 1) < 0) {
			perror(name);
			close(fd);
			continue;
		}
		close(fd);

		if (!strcmp(compat, buffer)) {
			/* Initialize these to "unset" for the way up. */
			*addr_cells_ptr = *size_cells_ptr = -1;

			/* Can't leave string on the stack or we'll lose it! */
			ret = strdup(parent);
			break;
		}
	}

	closedir(dir);
	return ret;
}
#endif /* defined(__arm__) || defined(__aarch64__) */

int main(int argc, char** argv)
{
	int print_defaults = 1;
	int print_console = 0;
	int print_coverage = 0;
	int print_list = 0;
	int print_hexdump = 0;
	int print_rawdump = 0;
	int print_timestamps = 0;
	int print_tcpa_log = 0;
	int machine_readable_timestamps = 0;
	enum console_print_type console_type = CONSOLE_PRINT_FULL;
	unsigned int rawdump_id = 0;

	int opt, option_index = 0;
	static struct option long_options[] = {
		{"console", 0, 0, 'c'},
		{"oneboot", 0, 0, '1'},
		{"2ndtolast", 0, 0, '2'},
		{"coverage", 0, 0, 'C'},
		{"list", 0, 0, 'l'},
		{"tcpa-log", 0, 0, 'L'},
		{"timestamps", 0, 0, 't'},
		{"parseable-timestamps", 0, 0, 'T'},
		{"hexdump", 0, 0, 'x'},
		{"rawdump", required_argument, 0, 'r'},
		{"verbose", 0, 0, 'V'},
		{"version", 0, 0, 'v'},
		{"help", 0, 0, 'h'},
		{0, 0, 0, 0}
	};
	while ((opt = getopt_long(argc, argv, "c12CltTLxVvh?r:",
				  long_options, &option_index)) != EOF) {
		switch (opt) {
		case 'c':
			print_console = 1;
			print_defaults = 0;
			break;
		case '1':
			print_console = 1;
			console_type = CONSOLE_PRINT_LAST;
			print_defaults = 0;
			break;
		case '2':
			print_console = 1;
			console_type = CONSOLE_PRINT_PREVIOUS;
			print_defaults = 0;
			break;
		case 'C':
			print_coverage = 1;
			print_defaults = 0;
			break;
		case 'l':
			print_list = 1;
			print_defaults = 0;
			break;
		case 'L':
			print_tcpa_log = 1;
			print_defaults = 0;
			break;
		case 'x':
			print_hexdump = 1;
			print_defaults = 0;
			break;
		case 'r':
			print_rawdump = 1;
			print_defaults = 0;
			rawdump_id = strtoul(optarg, NULL, 16);
			break;
		case 't':
			print_timestamps = 1;
			print_defaults = 0;
			break;
		case 'T':
			print_timestamps = 1;
			machine_readable_timestamps = 1;
			print_defaults = 0;
			break;
		case 'V':
			verbose = 1;
			break;
		case 'v':
			print_version();
			exit(0);
			break;
		case 'h':
			print_usage(argv[0], 0);
			break;
		case '?':
		default:
			print_usage(argv[0], 1);
			break;
		}
	}

	if (optind < argc) {
		fprintf(stderr, "Error: Extra parameter found.\n");
		print_usage(argv[0], 1);
	}

	mem_fd = open("/dev/mem", O_RDONLY, 0);
	if (mem_fd < 0) {
		fprintf(stderr, "Failed to gain memory access: %s\n",
			strerror(errno));
		return 1;
	}

#if defined(__arm__) || defined(__aarch64__)
	int addr_cells, size_cells;
	char *coreboot_node = dt_find_compat("/proc/device-tree", "coreboot",
					     &addr_cells, &size_cells);

	if (!coreboot_node) {
		fprintf(stderr, "Could not find 'coreboot' compatible node!\n");
		return 1;
	}

	if (addr_cells < 0) {
		fprintf(stderr, "Warning: no #address-cells node in tree!\n");
		addr_cells = 1;
	}

	int nlen = strlen(coreboot_node);
	char *reg = alloca(nlen + sizeof("/reg"));

	strcpy(reg, coreboot_node);
	strcpy(reg + nlen, "/reg");
	free(coreboot_node);

	int fd = open(reg, O_RDONLY);
	if (fd < 0) {
		perror(reg);
		return 1;
	}

	int i;
	size_t size_to_read = addr_cells * 4 + size_cells * 4;
	u8 *dtbuffer = alloca(size_to_read);
	if (read(fd, dtbuffer, size_to_read) < 0) {
		perror(reg);
		return 1;
	}
	close(fd);

	/* No variable-length byte swap function anywhere in C... how sad. */
	u64 baseaddr = 0;
	for (i = 0; i < addr_cells * 4; i++) {
		baseaddr <<= 8;
		baseaddr |= *dtbuffer;
		dtbuffer++;
	}
	u64 cb_table_size = 0;
	for (i = 0; i < size_cells * 4; i++) {
		cb_table_size <<= 8;
		cb_table_size |= *dtbuffer;
		dtbuffer++;
	}

	parse_cbtable(baseaddr, cb_table_size);
#else
	unsigned long long possible_base_addresses[] = { 0, 0xf0000 };

	/* Find and parse coreboot table */
	for (size_t j = 0; j < ARRAY_SIZE(possible_base_addresses); j++) {
		if (!parse_cbtable(possible_base_addresses[j], 0))
			break;
	}
#endif

	if (mapping_virt(&lbtable_mapping) == NULL)
		die("Table not found.\n");

	if (print_console)
		dump_console(console_type);

	if (print_coverage)
		dump_coverage();

	if (print_list)
		dump_cbmem_toc();

	if (print_hexdump)
		dump_cbmem_hex();

	if (print_rawdump)
		dump_cbmem_raw(rawdump_id);

	if (print_defaults || print_timestamps)
		dump_timestamps(machine_readable_timestamps);

	if (print_tcpa_log)
		dump_tcpa_log();

	unmap_memory(&lbtable_mapping);

	close(mem_fd);
	return 0;
}
