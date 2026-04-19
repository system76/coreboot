/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/pei_data.h>
#include <soc/pei_wrapper.h>

const struct usb2_port_setting mainboard_usb2_ports[MAX_USB2_PORTS] = {
	/* P0: HOST PORT */
	{ 0x0080, 1, 0, USB_PORT_BACK_PANEL },
	/* P1: HOST PORT */
	{ 0x0080, 1, 1, USB_PORT_BACK_PANEL },
	/* P2: RAIDEN */
	{ 0x0080, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL },
	/* P3: SD CARD */
	{ 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_INTERNAL },
	/* P4: RAIDEN */
	{ 0x0080, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL },
	/* P5: WWAN (Disabled) */
	{ 0x0000, 0, USB_OC_PIN_SKIP, USB_PORT_SKIP },
	/* P6: CAMERA */
	{ 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_INTERNAL },
	/* P7: BT */
	{ 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_INTERNAL },
};

const struct usb3_port_setting mainboard_usb3_ports[MAX_USB3_PORTS] = {
	/* P1: HOST PORT */
	{ 1, 0, 0 },
	/* P2: HOST PORT */
	{ 1, 1, 0 },
	/* P3: RAIDEN */
	{ 1, USB_OC_PIN_SKIP, 0 },
	/* P4: RAIDEN */
	{ 1, USB_OC_PIN_SKIP, 0 },
};
