/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/intel/haswell/haswell.h>
#include <cpu/intel/microcode.h>
#include <cpu/x86/msr.h>
#include <string.h>
#include <types.h>

/* FIXME: Needs an update */
static struct {
	u32 cpuid;
	const char *name;
} cpu_table[] = {
	{ CPUID_HASWELL_A0,       "Haswell A0" },
	{ CPUID_HASWELL_B0,       "Haswell B0" },
	{ CPUID_HASWELL_C0,       "Haswell C0" },
	{ CPUID_HASWELL_ULT_B0,   "Haswell ULT B0" },
	{ CPUID_HASWELL_ULT_C0,   "Haswell ULT C0 or D0" },
	{ CPUID_CRYSTALWELL_C0,   "Haswell Perf Halo" },
	{ CPUID_BROADWELL_ULT_C0, "Broadwell C0" },
	{ CPUID_BROADWELL_ULT_D0, "Broadwell D0" },
	{ CPUID_BROADWELL_ULT_E0, "Broadwell E0 or F0" },
};

void report_cpu_info(void)
{
	static const char *const mode[] = {"NOT ", ""};
	const char *cpu_type = "Unknown";

	char cpu_string[50];
	char *cpu_name = cpu_string; /* 48 bytes are reported */

	const u32 index = 0x80000000;
	struct cpuid_result cpuidr = cpuid(index);
	if (cpuidr.eax < 0x80000004) {
		strcpy(cpu_string, "Platform info not available");
	} else {
		u32 *p = (u32 *)cpu_string;
		for (unsigned int i = 2; i <= 4; i++) {
			cpuidr = cpuid(index + i);
			*p++ = cpuidr.eax;
			*p++ = cpuidr.ebx;
			*p++ = cpuidr.ecx;
			*p++ = cpuidr.edx;
		}
	}
	/* Skip leading spaces in CPU name string */
	while (cpu_name[0] == ' ')
		cpu_name++;

	const u32 cpu_id = cpu_get_cpuid();

	/* Look for string to match the name */
	for (unsigned int i = 0; i < ARRAY_SIZE(cpu_table); i++) {
		if (cpu_table[i].cpuid == cpu_id) {
			cpu_type = cpu_table[i].name;
			break;
		}
	}

	printk(BIOS_DEBUG, "CPU: %s\n", cpu_name);
	printk(BIOS_DEBUG, "CPU: ID %x, %s, ucode: %08x\n",
	       cpu_id, cpu_type, get_current_microcode_rev());

	const u32 cpu_feature_flag = cpu_get_feature_flags_ecx();
	const bool aes = (cpu_feature_flag & CPUID_AES);
	const bool txt = (cpu_feature_flag & CPUID_SMX);
	const bool vt  = (cpu_feature_flag & CPUID_VMX);
	printk(BIOS_DEBUG, "CPU: AES %ssupported, TXT %ssupported, VT %ssupported\n",
	       mode[aes], mode[txt], mode[vt]);
}
