/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/pei_data.h>
#include <soc/pei_wrapper.h>

const struct usb2_port_setting mainboard_usb2_ports[MAX_USB2_PORTS] = {
	/* P0: Port B, CN01 (IOBoard) */
	pei_data_usb2_port(pei_data, 0, 0x0150, 1, 0, USB_PORT_BACK_PANEL),
	/* P1: Port A, CN01 */
	pei_data_usb2_port(pei_data, 1, 0x0040, 1, 2, USB_PORT_BACK_PANEL),
	/* P2: CCD */
	pei_data_usb2_port(pei_data, 2, 0x0080, 1, USB_OC_PIN_SKIP, USB_PORT_INTERNAL),
	/* P3: BT */
	pei_data_usb2_port(pei_data, 3, 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_MINI_PCIE),
	/* P4: Empty */
	pei_data_usb2_port(pei_data, 4, 0x0000, 0, USB_OC_PIN_SKIP, USB_PORT_SKIP),
	/* P5: EMPTY */
	pei_data_usb2_port(pei_data, 5, 0x0000, 0, USB_OC_PIN_SKIP, USB_PORT_SKIP),
	/* P6: SD Card */
	pei_data_usb2_port(pei_data, 6, 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_FLEX),
	/* P7: EMPTY */
	pei_data_usb2_port(pei_data, 7, 0x0000, 0, USB_OC_PIN_SKIP, USB_PORT_SKIP),
};

const struct usb3_port_setting mainboard_usb3_ports[MAX_USB3_PORTS] = {
	/* P0: PORTB*/
	pei_data_usb3_port(pei_data, 0, 1, 0, 0),
	/* P1: PORTA */
	pei_data_usb3_port(pei_data, 1, 1, 2, 0),
	/* P2: EMPTY */
	pei_data_usb3_port(pei_data, 2, 0, USB_OC_PIN_SKIP, 0),
	/* P3: EMPTY */
	pei_data_usb3_port(pei_data, 3, 0, USB_OC_PIN_SKIP, 0),
};
