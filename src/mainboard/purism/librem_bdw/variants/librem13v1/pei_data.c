/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/pei_data.h>
#include <soc/pei_wrapper.h>

void mb_get_spd_map(struct spd_info *spdi)
{
	spdi->addresses[0] = 0x50;
}

const struct usb2_port_setting mainboard_usb2_ports[MAX_USB2_PORTS] = {
	/* P1: Left Side Port (USB2 only) */
	{ 0x0080, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL },
	/* P2: Right Side Port (USB2) */
	{ 0x0080, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL },
	/* P3: Empty */
	{ 0x0000, 0, USB_OC_PIN_SKIP, USB_PORT_SKIP },
	/* P4: Camera */
	{ 0x0080, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL },
	/* P5: Bluetooth */
	{ 0x0080, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL },
	/* P6: Empty */
	{ 0x0080, 0, USB_OC_PIN_SKIP, USB_PORT_SKIP },
	/* P7: Empty */
	{ 0x0080, 0, USB_OC_PIN_SKIP, USB_PORT_SKIP },
	/* P8: SD Card */
	{ 0x0080, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL },
};

const struct usb3_port_setting mainboard_usb3_ports[MAX_USB3_PORTS] = {
	/* P1: Empty */
	{ 0, USB_OC_PIN_SKIP, 0 },
	/* P2: Right Side Port (USB3) */
	{ 1, USB_OC_PIN_SKIP, 0 },
	/* P3: Empty */
	{ 0, USB_OC_PIN_SKIP, 0 },
	/* P4: Empty */
	{ 0, USB_OC_PIN_SKIP, 0 },
};
