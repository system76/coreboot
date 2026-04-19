/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <superio/nuvoton/common/hwm.h>

void nuvoton_hwm_enable_peci(uint16_t base, uint8_t tjmax)
{
	/* Bank 7: PECI agent config. */
	nuvoton_hwm_select_bank(base, 7);
	pnp_write_hwm5_index(base, 0x01, 0x85);	/* PECI_En | Is_PECI30 | Routine_En */
	pnp_write_hwm5_index(base, 0x02, 0x02);	/* Timing: 1.5 MHz, 75% duty */
	pnp_write_hwm5_index(base, 0x03, 0x10);	/* Enable Agent 0 only */
	pnp_write_hwm5_index(base, 0x04, 0x00);
	pnp_write_hwm5_index(base, 0x09, tjmax);

	/* Bank 0 CR AEh bit 0: enable Agent 0 PECI reading. */
	nuvoton_hwm_select_bank(base, 0);
	pnp_write_hwm5_index(base, 0xae, 1);
}

void nuvoton_hwm_set_sensor_type(uint16_t base,
				 enum nuvoton_temp_source src,
				 enum nuvoton_temp_sensor_type type)
{
	uint8_t bit = 1 << src;
	uint8_t cr5d, cr5e;

	nuvoton_hwm_select_bank(base, 0);
	cr5d = pnp_read_hwm5_index(base, 0x5d);
	cr5e = pnp_read_hwm5_index(base, 0x5e);

	if (type == NUVOTON_TEMP_SENSOR_DIODE) {
		cr5d |= bit;
		cr5e |= bit;
	} else {
		cr5d &= ~bit;
		cr5e &= ~bit;
	}

	pnp_write_hwm5_index(base, 0x5d, cr5d);
	pnp_write_hwm5_index(base, 0x5e, cr5e);
}

void nuvoton_hwm_enable_peci_calibration(uint16_t base)
{
	nuvoton_hwm_select_bank(base, 4);
	pnp_write_hwm5_index(base, 0xf8, 0x50);	/* 10 updates per interval, 1 C per step */
	pnp_write_hwm5_index(base, 0xfa, 0x51);	/* 1 s interval, enable */
}

void nuvoton_hwm_configure_fan(uint16_t base, const struct nuvoton_fan_curve *fan)
{
	size_t i;

	printk(BIOS_DEBUG, "Nuvoton HWM: configuring fan %s (bank %u)\n",
	       fan->name, fan->bank);

	nuvoton_hwm_select_bank(base, fan->bank);

	pnp_write_hwm5_index(base, NUVOTON_FAN_SOURCE, fan->source);

	for (i = 0; i < NUVOTON_FAN_CURVE_NPOINTS; i++)
		pnp_write_hwm5_index(base, NUVOTON_FAN_TEMP(i), fan->temp[i]);
	for (i = 0; i < NUVOTON_FAN_CURVE_NPOINTS; i++)
		pnp_write_hwm5_index(base, NUVOTON_FAN_DUTY(i), fan->duty[i]);

	pnp_write_hwm5_index(base, NUVOTON_FAN_CRIT_TEMP, fan->crit_temp);
	pnp_write_hwm5_index(base, NUVOTON_FAN_CRIT_DUTY_EN, fan->crit_duty_en);
	pnp_write_hwm5_index(base, NUVOTON_FAN_CRIT_DUTY, fan->crit_duty);
	pnp_write_hwm5_index(base, NUVOTON_FAN_CRIT_TEMP_TOLERANCE, fan->crit_temp_tolerance);

	pnp_write_hwm5_index(base, NUVOTON_FAN_STEP_UP_TIME, fan->step_up_time);
	pnp_write_hwm5_index(base, NUVOTON_FAN_STEP_DOWN_TIME, fan->step_down_time);
	pnp_write_hwm5_index(base, NUVOTON_FAN_DUTY_PER_STEP,
			     (fan->duty_per_step_up << 4) | fan->duty_per_step_down);

	pnp_write_hwm5_index(base, NUVOTON_FAN_MODE_TEMP_TOLERANCE,
			     (NUVOTON_FAN_MODE_SMARTFAN_IV << 4) | fan->temp_tolerance);
}
