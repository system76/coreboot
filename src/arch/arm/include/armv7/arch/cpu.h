/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ARCH_CPU_H__
#define __ARCH_CPU_H__

#include <stdint.h>
#include <device/device.h>

static inline void cpu_relax(void) { }

#define asmlinkage

struct cpu_driver {
	struct device_operations *ops;
	const struct cpu_device_id *id_table;
};

struct cpuinfo_arm {
	uint8_t    arm;            /* CPU family */
	uint8_t    arm_vendor;     /* CPU vendor */
	uint8_t    arm_model;
};

/* Primitives for CPU and MP cores. */

/* read Main Id register (MIDR) */
static inline uint32_t read_midr(void)
{
	uint32_t value;
	asm volatile ("mrc p15, 0, %0, c0, c0, 0" : "=r"(value));
	return value;
}

/* read Multiprocessor Affinity Register (MPIDR) */
static inline uint32_t read_mpidr(void)
{
	uint32_t value;
	asm volatile ("mrc p15, 0, %0, c0, c0, 5" : "=r"(value));
	return value;
}

/* read Auxiliary Control Register (ACTLR) */
static inline uint32_t read_actlr(void)
{
	uint32_t val = 0;
	asm volatile ("mrc p15, 0, %0, c1, c0, 1" : "=r"(val));
	return val;
}

/* write Auxiliary Control Register (ACTLR) */
static inline void write_actlr(uint32_t val)
{
	asm volatile ("mcr p15, 0, %0, c1, c0, 1" : : "r" (val));
}

/* wait for interrupt. */
static inline void wfi(void)
{
	asm volatile ("wfi" : : : "memory");
}

/* wait for event. */
static inline void wfe(void)
{
	asm volatile ("wfe");
}

/* set event (to bring up cores in WFE state). */
static inline void sev(void)
{
	asm volatile ("sev");
}

/* puts CPU into System mode and disable interrupts. */
static inline void set_system_mode(void)
{
	asm volatile("msr cpsr_c, %0" :: "r"(0x1f | 0xc0));
}

#endif /* __ARCH_CPU_H__ */
