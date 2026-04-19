/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/pei_data.h>
#include <soc/pei_wrapper.h>

void mb_get_spd_map(struct spd_info *spdi)
{
	spdi->addresses[0] = 0x51;
	spdi->addresses[2] = 0x51;
}

const struct usb2_port_setting mainboard_usb2_ports[MAX_USB2_PORTS] = {
	{ 0x40, 1, USB_OC_PIN_SKIP, USB_PORT_FRONT_PANEL },
	{ 0x40, 1, USB_OC_PIN_SKIP, USB_PORT_FRONT_PANEL },
	{ 0x40, 1, USB_OC_PIN_SKIP, USB_PORT_FRONT_PANEL },
	{ 0x40, 1, USB_OC_PIN_SKIP, USB_PORT_FRONT_PANEL },
	{ 0x40, 1, USB_OC_PIN_SKIP, USB_PORT_FRONT_PANEL },
	{ 0x40, 1, USB_OC_PIN_SKIP, USB_PORT_FRONT_PANEL },
	{ 0x40, 1, USB_OC_PIN_SKIP, USB_PORT_FRONT_PANEL },
	{ 0x40, 1, USB_OC_PIN_SKIP, USB_PORT_FRONT_PANEL },
};

const struct usb3_port_setting mainboard_usb3_ports[MAX_USB3_PORTS] = {
	{ 1, USB_OC_PIN_SKIP, 0 },
	{ 1, USB_OC_PIN_SKIP, 0 },
	{ 1, USB_OC_PIN_SKIP, 0 },
	{ 1, USB_OC_PIN_SKIP, 0 },
};
