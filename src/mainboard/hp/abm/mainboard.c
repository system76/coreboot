/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>

#include <southbridge/amd/common/amd_pci_util.h>
#include <southbridge/amd/agesa/hudson/pci_devs.h>
#include <northbridge/amd/agesa/family16kb/pci_devs.h>

#if 0
static const u8 mainboard_picr_data[0x54] = {
	0x03, 0x04, 0x05, 0x07, 0x0B, 0x0A, 0x1F, 0x1F, 0xFA, 0xF1, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F,
	0x1F, 0x1F, 0x1F, 0x03, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x05, 0x04, 0x05, 0x04, 0x04, 0x05, 0x04, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x04, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x03, 0x04, 0x05, 0x07
};
static const u8 mainboard_intr_data[0x54] = {
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F,
	0x09, 0x1F, 0x1F, 0x10, 0x1F, 0x10, 0x1F, 0x10, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x05, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x12, 0x11, 0x12, 0x11, 0x12, 0x11, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x11, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x10, 0x11, 0x12, 0x13
};
#endif

/***********************************************************
 * These arrays set up the FCH PCI_INTR registers 0xC00/0xC01.
 * This table is responsible for physically routing the PIC and
 * IOAPIC IRQs to the different PCI devices on the system.  It
 * is read and written via registers 0xC00/0xC01 as an
 * Index/Data pair.  These values are chipset and mainboard
 * dependent and should be updated accordingly.
 *
 * These values are used by the PCI configuration space,
 * MP Tables.  TODO: Make ACPI use these values too.
 */
static const u8 mainboard_picr_data[FCH_INT_TABLE_SIZE] = {
	[0x00] = 0x0A, 0x0B, 0x0A, 0x0B, 0x0A, 0x0B, 0x0A, 0x0B,  /* INTA# - INTH# */
	[0x08] = 0x00, 0xF1, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F,  /* Misc-nil, 0, 1, 2,  INT from Serial irq */
	[0x10] = 0x1F, 0x1F, 0x1F, 0x0A, 0x1F, 0x1F, 0x1F, 0x0A,  /* SCI,  SMBUS0,  ASF,  HDA,  FC,  GEC,  PerfMon,  SD */
	[0x20] = 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,            /* IMC INT0 - 5 */
	[0x30] = 0x0A, 0x0B, 0x0A, 0x0B, 0x0A, 0x0B, 0x0A,       /* USB Devs 18/19/20/22 INTA-C */
	[0x40] = 0x0B, 0x0B,                                /* IDE,  SATA */
};

static const u8 mainboard_intr_data[FCH_INT_TABLE_SIZE] = {
	[0x00] = 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,  /* INTA# - INTH# */
	[0x08] = 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F,  /* Misc-nil, 0, 1, 2,  INT from Serial irq */
	[0x10] = 0x09, 0x1F, 0x1F, 0x10, 0x1F, 0x12, 0x1F, 0x10,  /* SCI,  SMBUS0,  ASF,  HDA,  FC,  GEC,  PerMon,  SD */
	[0x20] = 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,            /* IMC INT0 - 5 */
	[0x30] = 0x12, 0x11, 0x12, 0x11, 0x12, 0x11, 0x12,       /* USB Devs 18/19/22/20 INTA-C */
	[0x40] = 0x11, 0x13,                                /* IDE,  SATA */
};

/*
 * This table defines the index into the picr/intr_data
 * tables for each device.  Any enabled device and slot
 * that uses hardware interrupts should have an entry
 * in this table to define its index into the FCH
 * PCI_INTR register 0xC00/0xC01.  This index will define
 * the interrupt that it should use.  Putting PIRQ_A into
 * the PIN A index for a device will tell that device to
 * use PIC IRQ 10 if it uses PIN A for its hardware INT.
 */
static const struct pirq_struct mainboard_pirq_data[] = {
	/* {PCI_devfn, {PIN A, PIN B, PIN C, PIN D}}, */
	{GFX_DEVFN,           {PIRQ_A, PIRQ_B, PIRQ_NC, PIRQ_NC}},      /* VGA:     01.0 */
	{NB_PCIE_PORT2_DEVFN, {PIRQ_A, PIRQ_B, PIRQ_C, PIRQ_D}},        /* NIC:     02.2 */
	{NB_PCIE_PORT3_DEVFN, {PIRQ_A, PIRQ_B, PIRQ_C, PIRQ_D}},        /* NIC:     02.3 */
	{NB_PCIE_PORT4_DEVFN, {PIRQ_A, PIRQ_B, PIRQ_C, PIRQ_D}},        /* NIC:     02.4 */
	{NB_PCIE_PORT5_DEVFN, {PIRQ_A, PIRQ_B, PIRQ_C, PIRQ_D}},        /* NIC:     02.5 */
	{SATA_DEVFN,          {PIRQ_SATA, PIRQ_NC, PIRQ_NC, PIRQ_NC}},  /* SATA:    11.0 */
	{OHCI1_DEVFN,         {PIRQ_OHCI1, PIRQ_NC, PIRQ_NC, PIRQ_NC}}, /* OHCI1:   12.0 */
	{EHCI1_DEVFN,         {PIRQ_NC, PIRQ_EHCI1, PIRQ_NC, PIRQ_NC}}, /* EHCI1:   12.2 */
	{OHCI2_DEVFN,         {PIRQ_OHCI2, PIRQ_NC, PIRQ_NC, PIRQ_NC}}, /* OHCI2:   13.0 */
	{EHCI2_DEVFN,         {PIRQ_NC, PIRQ_EHCI2, PIRQ_NC, PIRQ_NC}}, /* EHCI2:   13.2 */
	{SMBUS_DEVFN,         {PIRQ_SMBUS, PIRQ_NC, PIRQ_NC, PIRQ_NC}}, /* SMBUS:   14.0 */
	{HDA_DEVFN,           {PIRQ_HDA, PIRQ_NC, PIRQ_NC, PIRQ_NC}},   /* HDA:     14.2 */
	{SB_PCI_PORT_DEVFN,   {PIRQ_H, PIRQ_E, PIRQ_F, PIRQ_G}},        /* PCIB:    14.4 */
	{SD_DEVFN,            {PIRQ_SD, PIRQ_NC, PIRQ_NC, PIRQ_NC}},    /* SD:      14.7 */
	{OHCI3_DEVFN,         {PIRQ_OHCI3, PIRQ_NC, PIRQ_NC, PIRQ_NC}}, /* OHCI3:   16.0 */
	{EHCI3_DEVFN,         {PIRQ_NC, PIRQ_EHCI3, PIRQ_NC, PIRQ_NC}}, /* EHCI3:   16.2 */
};

/* PIRQ Setup */
static void pirq_setup(void)
{
	pirq_data_ptr = mainboard_pirq_data;
	pirq_data_size = ARRAY_SIZE(mainboard_pirq_data);
	intr_data_ptr = mainboard_intr_data;
	picr_data_ptr = mainboard_picr_data;
}

/**********************************************
 * Enable the dedicated functions of the board.
 **********************************************/
static void mainboard_enable(struct device *dev)
{
	/* Initialize the PIRQ data structures for consumption */
	pirq_setup();
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
