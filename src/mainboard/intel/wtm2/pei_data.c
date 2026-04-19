/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/pei_data.h>
#include <soc/pei_wrapper.h>

void mb_get_spd_map(struct spd_info *spdi)
{
	spdi->addresses[0] = 0x51;
	spdi->addresses[2] = 0x51;
}

const struct usb2_port_setting mainboard_usb2_ports[MAX_USB2_PORTS] = {
	pei_data_usb2_port(pei_data, 0, 0x40, 1, USB_OC_PIN_SKIP, USB_PORT_FRONT_PANEL),
	pei_data_usb2_port(pei_data, 1, 0x40, 1, USB_OC_PIN_SKIP, USB_PORT_FRONT_PANEL),
	pei_data_usb2_port(pei_data, 2, 0x40, 1, USB_OC_PIN_SKIP, USB_PORT_FRONT_PANEL),
	pei_data_usb2_port(pei_data, 3, 0x40, 1, USB_OC_PIN_SKIP, USB_PORT_FRONT_PANEL),
	pei_data_usb2_port(pei_data, 4, 0x40, 1, USB_OC_PIN_SKIP, USB_PORT_FRONT_PANEL),
	pei_data_usb2_port(pei_data, 5, 0x40, 1, USB_OC_PIN_SKIP, USB_PORT_FRONT_PANEL),
	pei_data_usb2_port(pei_data, 6, 0x40, 1, USB_OC_PIN_SKIP, USB_PORT_FRONT_PANEL),
	pei_data_usb2_port(pei_data, 7, 0x40, 1, USB_OC_PIN_SKIP, USB_PORT_FRONT_PANEL),
};

const struct usb3_port_setting mainboard_usb3_ports[MAX_USB3_PORTS] = {
	pei_data_usb3_port(pei_data, 0, 1, USB_OC_PIN_SKIP, 0),
	pei_data_usb3_port(pei_data, 1, 1, USB_OC_PIN_SKIP, 0),
	pei_data_usb3_port(pei_data, 2, 1, USB_OC_PIN_SKIP, 0),
	pei_data_usb3_port(pei_data, 3, 1, USB_OC_PIN_SKIP, 0),
};
