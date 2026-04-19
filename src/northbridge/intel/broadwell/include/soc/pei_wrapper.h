/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BROADWELL_PEI_WRAPPER_H_
#define _BROADWELL_PEI_WRAPPER_H_

#include <soc/pei_data.h>
#include <string.h>

typedef int ABI_X86(*pei_wrapper_entry_t)(struct pei_data *pei_data);

/* TODO: Should be moved to PCH code */
extern const struct usb2_port_setting mainboard_usb2_ports[MAX_USB2_PORTS];
extern const struct usb3_port_setting mainboard_usb3_ports[MAX_USB3_PORTS];

#define SPD_MEMORY_DOWN	0xff

struct spd_info {
	uint8_t addresses[4];
	unsigned int spd_index;
};

struct lpddr3_dq_dqs_map {
	uint8_t dq[2][6][2];
	uint8_t dqs[2][8];
};

/* Mainboard callback to fill in the SPD addresses */
void mb_get_spd_map(struct spd_info *spdi);

/* Mainboard callback to retrieve the LPDDR3-specific DQ/DQS mapping */
const struct lpddr3_dq_dqs_map *mb_get_lpddr3_dq_dqs_map(void);

void broadwell_fill_pei_data(struct pei_data *pei_data);

void copy_spd(struct pei_data *pei_data, struct spd_info *spdi);

#endif
