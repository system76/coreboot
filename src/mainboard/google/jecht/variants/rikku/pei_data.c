/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/pei_data.h>
#include <soc/pei_wrapper.h>

const struct usb2_port_setting mainboard_usb2_ports[MAX_USB2_PORTS] = {
	/* P0: VP8 */
	pei_data_usb2_port(pei_data, 0, 0x0064, 1, 0, USB_PORT_MINI_PCIE),
	/* P1: Port A, CN22 */
	pei_data_usb2_port(pei_data, 1, 0x0040, 1, 0, USB_PORT_INTERNAL),
	/* P2: Port B, CN23 */
	pei_data_usb2_port(pei_data, 2, 0x0040, 1, 1, USB_PORT_INTERNAL),
	/* P3: WLAN */
	pei_data_usb2_port(pei_data, 3, 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_MINI_PCIE),
	/* P4: Port C, CN25 */
	pei_data_usb2_port(pei_data, 4, 0x0040, 1, 2, USB_PORT_INTERNAL),
	/* P5: Port D, CN25 */
	pei_data_usb2_port(pei_data, 5, 0x0040, 1, 2, USB_PORT_INTERNAL),
	/* P6: Card Reader */
	pei_data_usb2_port(pei_data, 6, 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_INTERNAL),
	/* P7: EMPTY */
	pei_data_usb2_port(pei_data, 7, 0x0000, 0, 0, USB_PORT_SKIP),
};

const struct usb3_port_setting mainboard_usb3_ports[MAX_USB3_PORTS] = {
	/* P1: CN22 */
	pei_data_usb3_port(pei_data, 0, 1, 0, 0),
	/* P2: CN23 */
	pei_data_usb3_port(pei_data, 1, 1, 1, 0),
	/* P3: CN25 */
	pei_data_usb3_port(pei_data, 2, 1, 2, 0),
	/* P4: CN25 */
	pei_data_usb3_port(pei_data, 3, 1, 2, 0),
};
