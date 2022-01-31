/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_ALDERLAKE_CPU_H_
#define _SOC_ALDERLAKE_CPU_H_

/* Latency times in us */
#define C1_LATENCY	1
#define C6_LATENCY	127
#define C7_LATENCY	253
#define C8_LATENCY	260
#define C9_LATENCY	487
#define C10_LATENCY	1048

/* Power in units of mW */
#define C1_POWER	0x3e8
#define C6_POWER	0x15e
#define C7_POWER	0xc8
#define C8_POWER	0xc8
#define C9_POWER	0xc8
#define C10_POWER	0xc8

enum adl_cpu_type {
	ADL_UNKNOWN,
	ADL_M,
	ADL_N,
	ADL_P,
	ADL_S,
};

enum adl_cpu_type get_adl_cpu_type(void);

/* Get a bitmask of supported LPM states */
uint8_t get_supported_lpm_mask(void);

#endif
