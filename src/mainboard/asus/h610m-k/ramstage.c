/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <soc/ramstage.h>
#include <superio/nuvoton/common/hwm.h>

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
}

/* HWM base address, set in devicetree.cb. */
#define HWM_IOBASE		0x290

/* Alder Lake desktop Tjmax in degrees Celsius. */
#define CPU_TJMAX		100

/* Per-fan register banks. Only SYSFAN and CPUFAN are wired up. */
#define BANK_SYSFAN		1
#define BANK_CPUFAN		2

/* Fan profiles. The curves below were picked empirically on an i3-12100. */
static const struct nuvoton_fan_curve fans[] = {
	{
		.name			= "SYSFAN",
		.bank			= BANK_SYSFAN,
		.source			= NUVOTON_FAN_SOURCE_PECI0_CAL,
		.temp			= { 40, 60, 80, 100 },
		.duty			= {
			NUVOTON_PERCENT_TO_DUTY(20),
			NUVOTON_PERCENT_TO_DUTY(40),
			NUVOTON_PERCENT_TO_DUTY(60),
			NUVOTON_PERCENT_TO_DUTY(80),
		},
		.crit_temp		= 125,
		.crit_duty_en		= 1,
		.crit_duty		= NUVOTON_PERCENT_TO_DUTY(100),
		.crit_temp_tolerance	= 2,
	},
	{
		.name			= "CPUFAN",
		.bank			= BANK_CPUFAN,
		.source			= NUVOTON_FAN_SOURCE_PECI0_CAL,
		.temp			= { 40, 60, 80, 100 },
		.duty			= {
			NUVOTON_PERCENT_TO_DUTY(20),
			NUVOTON_PERCENT_TO_DUTY(40),
			NUVOTON_PERCENT_TO_DUTY(60),
			NUVOTON_PERCENT_TO_DUTY(80),
		},
		.crit_temp		= 125,
		.crit_duty_en		= 1,
		.crit_duty		= NUVOTON_PERCENT_TO_DUTY(100),
		.crit_temp_tolerance	= 2,
	},
};

static void hwm_init(void *arg)
{
	/*
	 * The board wires a real thermistor to CPUTIN (not a diode), which
	 * PECI calibration uses as its reference. Chip default is diode +
	 * current mode; reading that without configuring CPUTIN correctly
	 * leaves the calibrated source garbage.
	 */
	nuvoton_hwm_set_sensor_type(HWM_IOBASE, NUVOTON_TEMP_SRC_CPUTIN,
				    NUVOTON_TEMP_SENSOR_THERMISTOR);

	/* Devicetree must set superio pin 121 to PECI mode first. */
	nuvoton_hwm_enable_peci(HWM_IOBASE, CPU_TJMAX);
	nuvoton_hwm_enable_peci_calibration(HWM_IOBASE);

	for (size_t i = 0; i < ARRAY_SIZE(fans); i++)
		nuvoton_hwm_configure_fan(HWM_IOBASE, &fans[i]);
}

BOOT_STATE_INIT_ENTRY(BS_POST_DEVICE, BS_ON_EXIT, hwm_init, NULL);
