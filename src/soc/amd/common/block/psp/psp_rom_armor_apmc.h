/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_PSP_ROM_ARMOR_APMC_H
#define AMD_BLOCK_PSP_ROM_ARMOR_APMC_H

#include <stddef.h>
#include <stdint.h>

/* ROM Armor APM sub-commands */
enum rom_armor_apm_cmd {
	ROM_ARMOR_APM_CMD_INIT = 1,
	ROM_ARMOR_APM_CMD_READ = 2,
	ROM_ARMOR_APM_CMD_WRITE = 3,
	ROM_ARMOR_APM_CMD_ERASE = 4,
	ROM_ARMOR_APM_CMD_SHUTDOWN = 5,
};

/* Return values */
enum rom_armor_apm_result {
	ROM_ARMOR_RET_SUCCESS = 0,
	ROM_ARMOR_RET_FAILURE,
	ROM_ARMOR_RET_SHUTDOWN,
	ROM_ARMOR_RET_NOT_INITIALIZED,
	ROM_ARMOR_RET_UNSUPPORTED,
};

/* Parameter structures for ROM Armor operations */
struct rom_armor_params_init {
	bool capsule_update;	/* 1 for capsule update/recovery mode, 0 otherwise */
};

struct rom_armor_params_read {
	void *buf;
	size_t offset;
	size_t size;
};

struct rom_armor_params_write {
	const void *buf;
	size_t offset;
	size_t size;
};

struct rom_armor_params_erase {
	size_t offset;
	size_t size;
};

#endif /* AMD_BLOCK_PSP_ROM_ARMOR_APMC_H */
