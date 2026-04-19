/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/pei_data.h>
#include <soc/pei_wrapper.h>

const struct usb2_port_setting mainboard_usb2_ports[MAX_USB2_PORTS] = {
	/* P0: LTE */
	{ 0x0150, 1, USB_OC_PIN_SKIP, USB_PORT_MINI_PCIE },
	/* P1: POrt A, CN10 */
	{ 0x0040, 1, 0, USB_PORT_BACK_PANEL },
	/* P2: CCD */
	{ 0x0080, 1, USB_OC_PIN_SKIP, USB_PORT_INTERNAL },
	/* P3: BT */
	{ 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_MINI_PCIE },
	/* P4: Port B, CN6 */
	{ 0x0040, 1, 2, USB_PORT_BACK_PANEL },
	/* P5: EMPTY */
	{ 0x0000, 0, USB_OC_PIN_SKIP, USB_PORT_SKIP },
	/* P6: SD Card */
	{ 0x0150, 1, USB_OC_PIN_SKIP, USB_PORT_FLEX },
	/* P7: EMPTY */
	{ 0x0000, 0, USB_OC_PIN_SKIP, USB_PORT_SKIP },
};

const struct usb3_port_setting mainboard_usb3_ports[MAX_USB3_PORTS] = {
	/* P1: Port A, CN6 */
	{ 1, 0, 0 },
	/* P2: EMPTY */
	{ 0, USB_OC_PIN_SKIP, 0 },
	/* P3: EMPTY */
	{ 0, USB_OC_PIN_SKIP, 0 },
	/* P4: EMPTY */
	{ 0, USB_OC_PIN_SKIP, 0 },
};
