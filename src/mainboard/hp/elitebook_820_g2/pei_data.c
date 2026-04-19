/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/pei_data.h>
#include <soc/pei_wrapper.h>

void mb_get_spd_map(struct spd_info *spdi)
{
	spdi->addresses[0] = 0x50;
	spdi->addresses[2] = 0x52;
}

const struct usb2_port_setting mainboard_usb2_ports[MAX_USB2_PORTS] = {
	/* P1 */
	{ 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL },
	/* P2: left side port, USB debug */
	{ 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL },
	/* P3: digitizer and right side ports (Microchip hub) */
	{ 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL },
	/* P4: WLAN */
	{ 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL },
	/* P5: fingerprint reader */
	{ 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL },
	/* P6: WWAN */
	{ 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL },
	/* P7: webcam */
	{ 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL },
	/* P8 */
	{ 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL },
};

const struct usb3_port_setting mainboard_usb3_ports[MAX_USB3_PORTS] = {
	/* P1 */
	{ 1, USB_OC_PIN_SKIP, 0 },
	/* P2: left side, USB debug */
	{ 1, USB_OC_PIN_SKIP, 0 },
	/* P3: right side (Microchip hub) */
	{ 1, USB_OC_PIN_SKIP, 0 },
	/* P4 */
	{ 1, USB_OC_PIN_SKIP, 0 },
};
