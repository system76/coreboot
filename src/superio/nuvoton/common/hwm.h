/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SUPERIO_NUVOTON_COMMON_HWM_H
#define SUPERIO_NUVOTON_COMMON_HWM_H

/* Nuvoton is a Winbond spin-off, so this code is for both */

#include <stdint.h>
#include <superio/hwm5_conf.h>

#define HWM_BANK_SELECT	0x4e

static inline void nuvoton_hwm_select_bank(const u16 base, const u8 bank)
{
	pnp_write_hwm5_index(base, HWM_BANK_SELECT, bank);
}

/* SmartFan IV register offsets within a per-fan bank. */

/* Fan control temperature source. */
#define NUVOTON_FAN_SOURCE			0x00
/* PECI agent raw; PECI_CAL is the CPUTIN-calibrated version. */
#define NUVOTON_FAN_SOURCE_PECI0		0x10
#define NUVOTON_FAN_SOURCE_PECI1		0x11
#define NUVOTON_FAN_SOURCE_PECI0_CAL		0x1c
#define NUVOTON_FAN_SOURCE_PECI1_CAL		0x1d

/* Fan mode + temperature tolerance: [7:4] mode, [2:0] tolerance */
#define NUVOTON_FAN_MODE_TEMP_TOLERANCE		0x02
#define NUVOTON_FAN_MODE_SMARTFAN_IV		4

/* Step up/down time, in 0.1 second units */
#define NUVOTON_FAN_STEP_UP_TIME		0x03
#define NUVOTON_FAN_STEP_DOWN_TIME		0x04

/* Duty change per step: [7:4] up, [3:0] down */
#define NUVOTON_FAN_DUTY_PER_STEP		0x66

/* 4-point SmartFan IV curve */
#define NUVOTON_FAN_TEMP(i)			(0x21 + (i))
#define NUVOTON_FAN_DUTY(i)			(0x27 + (i))

/* Critical temperature handling */
#define NUVOTON_FAN_CRIT_TEMP			0x35
#define NUVOTON_FAN_CRIT_DUTY_EN		0x36
#define NUVOTON_FAN_CRIT_DUTY			0x37
#define NUVOTON_FAN_CRIT_TEMP_TOLERANCE		0x38

#define NUVOTON_PERCENT_TO_DUTY(perc)		((perc) * 255 / 100)

#define NUVOTON_FAN_CURVE_NPOINTS		4

struct nuvoton_fan_curve {
	const char *name;

	/* Per-fan register bank (1 = SYSFAN, 2 = CPUFAN, ...) */
	uint8_t bank;

	/* Temperature source (NUVOTON_FAN_SOURCE_*) */
	uint8_t source;

	/* Temperature vs duty curve */
	uint8_t temp[NUVOTON_FAN_CURVE_NPOINTS];
	uint8_t duty[NUVOTON_FAN_CURVE_NPOINTS];

	/* Temperature tolerance */
	uint8_t temp_tolerance;

	/* Step up/down smoothing */
	uint8_t step_up_time;
	uint8_t step_down_time;
	uint8_t duty_per_step_up;
	uint8_t duty_per_step_down;

	/* Critical mode */
	uint8_t crit_temp;
	uint8_t crit_duty_en;
	uint8_t crit_duty;
	uint8_t crit_temp_tolerance;
};

/*
 * Temperature sensor input channels. The numeric value of each enum
 * matches the bit position in Bank 0 CR 5Dh (sensor type) and Bank 0
 * CR 5Eh (current-mode enable) for that channel.
 */
enum nuvoton_temp_source {
	NUVOTON_TEMP_SRC_SYSTIN  = 1,
	NUVOTON_TEMP_SRC_CPUTIN  = 2,
	NUVOTON_TEMP_SRC_AUXTIN0 = 3,
	NUVOTON_TEMP_SRC_AUXTIN1 = 4,
	NUVOTON_TEMP_SRC_AUXTIN2 = 5,
	NUVOTON_TEMP_SRC_AUXTIN3 = 6,
	NUVOTON_TEMP_SRC_AUXTIN4 = 7,
};

enum nuvoton_temp_sensor_type {
	NUVOTON_TEMP_SENSOR_THERMISTOR,	/* voltage-divider mode */
	NUVOTON_TEMP_SENSOR_DIODE,	/* current-sensing mode (external diode) */
};

/*
 * Configure the sensor type for one temperature input channel. Boards
 * must call this before the relevant channel's reading is meaningful
 * if the chip default (diode + current mode, except for SYSTIN which
 * defaults to thermistor) does not match the hardware actually wired
 * to the pin.
 */
void nuvoton_hwm_set_sensor_type(uint16_t base,
				 enum nuvoton_temp_source src,
				 enum nuvoton_temp_sensor_type type);

/*
 * Enable PECI Agent 0 on the HWM block as a readable temperature source.
 *
 * tjmax is the CPU's Tjmax in degrees Celsius (100 on most Intel
 * desktop parts); PECI reports temperature as an offset below this.
 * Devicetree must have configured the superio pin as PECI first.
 */
void nuvoton_hwm_enable_peci(uint16_t base, uint8_t tjmax);

/*
 * Enable the PECI calibration pass (NCT6796D datasheet s8.7.1). The
 * chip slews the calibrated source toward CPUTIN in bounded steps.
 * Boards with a real thermistor on CPUTIN must configure it via
 * nuvoton_hwm_set_sensor_type beforehand, or the calibrated output
 * will be garbage.
 */
void nuvoton_hwm_enable_peci_calibration(uint16_t base);

/* Program SmartFan IV on one fan using the given curve. */
void nuvoton_hwm_configure_fan(uint16_t base, const struct nuvoton_fan_curve *fan);

#endif /* SUPERIO_NUVOTON_COMMON_HWM_H */
