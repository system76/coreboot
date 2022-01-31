/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/smm.h>
#include <device/device.h>
#include <drivers/intel/gma/int15.h>
#include <acpi/acpi.h>
#include <southbridge/intel/lynxpoint/pch.h>

void mainboard_suspend_resume(void)
{
	/* Call SMM finalize() handlers before resume */
	apm_control(APM_CNT_FINALIZE);
}

// mainboard_enable is executed as first thing after
// enumerate_buses().

static void mainboard_enable(struct device *dev)
{
	install_intel_vga_int15_handler(GMA_INT15_ACTIVE_LFP_EDP, GMA_INT15_PANEL_FIT_CENTERING, GMA_INT15_BOOT_DISPLAY_DEFAULT, 0);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
