/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <acpi/acpi.h>
#include <cpu/x86/smm.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/common/ite_gpio.h>
#include <superio/ite/it8772f/it8772f.h>

#define GPIO_DEV PNP_DEV(0x2e, IT8772F_GPIO)

void mainboard_smi_sleep(u8 slp_typ)
{
	switch (slp_typ) {
	case ACPI_S3:
		ite_enable_3vsbsw(GPIO_DEV);
		ite_delay_pwrgd3(GPIO_DEV);
		break;
	default:
		break;
	}
}
