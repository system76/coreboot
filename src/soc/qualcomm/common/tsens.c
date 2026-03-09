/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/mmio.h>
#include <commonlib/bsd/stdlib.h>
#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/qcom_tsens.h>

#if CONFIG(SOC_QUALCOMM_DEBUG_TSENS)
#define thermal_printk(fmt, ...) printk(BIOS_INFO, fmt, ##__VA_ARGS__)
#else
#define thermal_printk(fmt, ...) do { (void)(fmt); } while (0)
#endif

static int read_tsens(const struct thermal_zone_map *zone, int *out)
{
	if (!zone)
		return -1;

	const struct tsens_controller *ctrl = (const struct tsens_controller *)zone->ctrl;
	if (!ctrl || (zone->hw_id >= (int)ctrl->sensor_count))
		return -1;

	uintptr_t status_addr = ctrl->tm_base + TM_SN_STATUS_OFF + (zone->hw_id * 4);

	uint32_t status = read32p(status_addr);
	if (!(status & SN_STATUS_VALID))
		return -2;

	/* Sign extend 12-bit temperature and scale to milli C */
	*out = ((int32_t)((status & SN_STATUS_TEMP_MASK) << 20) >> 20) * 100;
	return 0;
}

void qcom_tsens_monitor_all(bool *has_crossed_threshold)
{
	if (!has_crossed_threshold)
		return;

	*has_crossed_threshold = false;

	thermal_printk("\n------------------ Thermal Sensor State --------------------\n");
	thermal_printk("%-15s | %-10s | %-8s | %-10s\n", "Zone", "Type", "Temp (C)", "Status");
	thermal_printk("------------------------------------------------------------\n");

	for (const struct thermal_zone_map *zone = qcom_thermal_zones; zone->label != NULL; zone++) {
		int data = 0, status = -1;
		const char *type_str = "UNK";
		(void)type_str;

		if (zone->type == TYPE_TSENS) {
			type_str = "SoC";
			status = read_tsens(zone, &data);
		}

		if (status == 0) {
			thermal_printk("%-15s | %-10s | %3d.%1d    | ",
			       zone->label, type_str, data / 1000, (data % 1000) / 100);

			if (data >= zone->threshold) {
				*has_crossed_threshold = true;
				thermal_printk("EXCEEDED\n");
				if (!CONFIG(SOC_QUALCOMM_DEBUG_TSENS))
					return;
			} else {
				thermal_printk("OK\n");
			}
		} else {
			thermal_printk("%-15s | %-10s | [ERR %d]  | -\n", zone->label, type_str, status);
		}
	}
}
