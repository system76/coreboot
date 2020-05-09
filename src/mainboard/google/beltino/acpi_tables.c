/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <types.h>
#include <acpi/acpi.h>
#include <arch/smp/mpspec.h>
#include <device/device.h>
#include <ec/google/chromeec/ec.h>
#include <southbridge/intel/lynxpoint/nvs.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <vendorcode/google/chromeos/gnvs.h>
#include <variant/thermal.h>

void acpi_create_gnvs(global_nvs_t *gnvs)
{
	/* Enable USB ports in S3 */
	gnvs->s3u0 = 1;
	gnvs->s3u1 = 1;

	/* Disable USB ports in S5 */
	gnvs->s5u0 = 0;
	gnvs->s5u1 = 0;

	/* TPM Present */
	gnvs->tpmp = 1;

#if CONFIG(CHROMEOS)
	// SuperIO is always RO
	gnvs->chromeos.vbt2 = ACTIVE_ECFW_RO;
#endif

	gnvs->f4of = FAN4_THRESHOLD_OFF;
	gnvs->f4on = FAN4_THRESHOLD_ON;
	gnvs->f4pw = FAN4_PWM;

	gnvs->f3of = FAN3_THRESHOLD_OFF;
	gnvs->f3on = FAN3_THRESHOLD_ON;
	gnvs->f3pw = FAN3_PWM;

	gnvs->f2of = FAN2_THRESHOLD_OFF;
	gnvs->f2on = FAN2_THRESHOLD_ON;
	gnvs->f2pw = FAN2_PWM;

	gnvs->f1of = FAN1_THRESHOLD_OFF;
	gnvs->f1on = FAN1_THRESHOLD_ON;
	gnvs->f1pw = FAN1_PWM;

	gnvs->f0of = FAN0_THRESHOLD_OFF;
	gnvs->f0on = FAN0_THRESHOLD_ON;
	gnvs->f0pw = FAN0_PWM;

	gnvs->tcrt = CRITICAL_TEMPERATURE;
	gnvs->tpsv = PASSIVE_TEMPERATURE;
	gnvs->tmax = MAX_TEMPERATURE;
	gnvs->flvl = 5;
}
