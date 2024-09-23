/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SUPERIO_NUVOTON_NCT5525D_H
#define SUPERIO_NUVOTON_NCT5525D_H

/* Logical Device Numbers (LDN) */
#define NCT5525D_UARTA		0x02 /* UART A */
#define NCT5525D_KBC		0x05 /* Keyboard Controller */
#define NCT5525D_CIR		0x06 /* Consumer IR */
#define NCT5525D_GPIO056	0x07 /* GPIO 0, 5, 6 */
#define NCT5525D_GPIO_WDT1	0x08 /* GPIO, WDT1 */
#define NCT5525D_GPIO89AB	0x09 /* GPIO 8, 9, A, B */
#define NCT5525D_ACPI		0x0A /* ACPI */
#define NCT5525D_HWM_FPLED	0x0B /* HW Monitor, Front Panel LED */
#define NCT5525D_WDT2		0x0D /* WDT2 */
#define NCT5525D_CIRWUP		0x0E /* CIR Wake-Up */
#define NCT5525D_GPIO_PP_OD	0x0F /* GPIO Pull-Pull/Open-Drain */
#define NCT5525D_UARTC		0x10 /* UART C */
#define NCT5525D_P80_IR		0x14 /* Port 80, IR */
#define NCT5525D_FLED		0x15 /* Fading LED */
#define NCT5525D_DS		0x16 /* Deep Sleep */

#endif /* SUPERIO_NUVOTON_NCT5525D_H */
