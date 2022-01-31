/* SPDX-License-Identifier: GPL-2.0-only */

OperationRegion (ECF2, EmbeddedControl, 0x00, 0x100)
Field (ECF2, ByteAcc, Lock, Preserve)
{
	Offset(0x00),
	ECMV, 8,	// Major Version Number
	ECSV, 8,	// Minor Version Number
	KBVS, 8,	// Keyboard Controller Version
	ECTV, 8,	// Test Version Number
	FRMF, 8,	// Force Mirror Flag
	TXEL, 8,	// TXE Lock

	Offset(0x10),
	CPWR, 8,	// Control Power
	CDEV, 8,	// Control Device
	OSFG, 8,	// OS Flag
	CWFU, 8,	// CW2015 Full
	TPLE, 8,	// Trackpad State
	AFG3, 8,	// After G3
	CLTP, 8,	// Close Trackpad
	WKOL, 8,	// Wake on Lid
	KLSE, 8,	// Keyboard Backlight State
	KLBE, 8,	// Keyboard Backlight Brightness

	Offset(0x1a),
	KLTE, 8,	// Keyboard Backlight Timeout

	Offset(0x22),
	ECT0, 8,	// EC Build Time 0
	ECT1, 8,	// EC Build Time 1
	ECT2, 8,	// EC Build Time 2
	ECT3, 8,	// EC Build Time 3
	ECT4, 8,	// EC Build Time 4
	ECT5, 8,	// EC Build Time 5
	ECT6, 8,	// EC Build Time 6
	ECT7, 8,	// EC Build Time 7
	ECT8, 8,	// EC Build Time 8
	ECT9, 8,	// EC Build Time 9

	Offset(0x2c),
	FLKE, 8,	// Function Lock State

	Offset(0x30),
	STEF, 8,	// Sensor T Error F
	ECD0, 8,	// EC Build Date 0
	ECD1, 8,	// EC Build Date 1
	ECD2, 8,	// EC Build Date 2
	ECD3, 8,	// EC Build Date 3
	ECD4, 8,	// EC Build Date 4
	ECD5, 8,	// EC Build Date 5
	ECD6, 8,	// EC Build Date 6
	ECD7, 8,	// EC Build Date 7
	ECD8, 8,	// EC Build Date 8
	ECD9, 8,	// EC Build Date 9

	Offset(0x40),
	SHIP, 8,        // Shipping Mode Flag
	LEDF, 8,	// LED Control Flag
	LIDF, 8,	// Lid Flag
	KBFL, 8,	// Keyboard Flag
	CYCC, 8,	// Cycle Count

	Offset(0x46),
	BFCP, 8,	// Battery Full Charge Percentage

	Offset(0x62),
	TSE2, 8,	// Sensor 2 Temperature
	SENF, 8,	// Sensor F
	TSHT, 8,	// Thermal Sensor High Trip Point
	TSLT, 8,	// Thermal Sensor Low Trip Point
	THER, 8,	// Thermal Source

	Offset(0x70),
	CPUT, 8,	// PECI CPU Temperature
	PMXT, 8,	// PLMX Temperature
	CHAR, 8,	// Charger Temperature

	Offset(0x7e),
	OCTF, 8,	// OEM Control Flag
	LSTE, 1,	// Lid Status
	    , 7,	// Reserved

	Offset(0x80),
	ECPS, 8,	// AC & Battery status
	B1MN, 8,	// Battery Model Number Code
	B1SN, 16,	// Battery Serial Number
	B1DC, 16,	// Battery Design Capacity
	B1DV, 16,	// Battery Design Voltage
	B1FC, 16,	// Battery Last Full Charge Capacity
	B1TP, 16,	// Battery Trip Point
	B1ST, 8,	// Battery State
	B1PR, 16,	// Battery Present Rate
	B1RC, 16,	// Battery Remaining Capacity
	B1PV, 16,	// Battery Present Voltage
	BPRP, 8,	// Battery Remaining percentage
	BT1A, 8,	// Bt1 ASOC
	BT1T, 16,	// Bt1 Temperature
	BT1C, 8,	// Bt1 Control

	// Unicorn - doesn't actually exist
	Offset(0x9d),
	OPWE, 8,	// OPM write to EC flag for UCSI
	// Unicorn - doesn't actually exist

	Offset(0xbf),
	EJ8A, 8,	// EJ898A Firmware Version

	Offset(0xc0),
	MGI0, 8,	// UCSI DS MGI 0
	MGI1, 8,	// UCSI DS MGI 1
	MGI2, 8,	// UCSI DS MGI 2
	MGI3, 8,	// UCSI DS MGI 3
	MGI4, 8,	// UCSI DS MGI 4
	MGI5, 8,	// UCSI DS MGI 5
	MGI6, 8,	// UCSI DS MGI 6
	MGI7, 8,	// UCSI DS MGI 7
	MGI8, 8,	// UCSI DS MGI 8
	MGI9, 8,	// UCSI DS MGI 9
	MGIA, 8,	// UCSI DS MGI A
	MGIB, 8,	// UCSI DS MGI B
	MGIC, 8,	// UCSI DS MGI C
	MGID, 8,	// UCSI DS MGI D
	MGIE, 8,	// UCSI DS MGI E
	MGIF, 8,	// UCSI DS MGI F

	Offset(0xd0),
	MGO0, 8,	// UCSI DS MGO 0
	MGO1, 8,	// UCSI DS MGO 1
	MGO2, 8,	// UCSI DS MGO 2
	MGO3, 8,	// UCSI DS MGO 3
	MGO4, 8,	// UCSI DS MGO 4
	MGO5, 8,	// UCSI DS MGO 5
	MGO6, 8,	// UCSI DS MGO 6
	MGO7, 8,	// UCSI DS MGO 7
	MGO8, 8,	// UCSI DS MGO 8
	MGO9, 8,	// UCSI DS MGO 9
	MGOA, 8,	// UCSI DS MGO A
	MGOB, 8,	// UCSI DS MGO B
	MGOC, 8,	// UCSI DS MGO C
	MGOD, 8,	// UCSI DS MGO D
	MGOE, 8,	// UCSI DS MGO E
	MGOF, 8,	// UCSI DS MGO F

	Offset(0xe0),
	UCSV, 16,	// UCSI DS Version
	UCSD, 16,	// UCSI DS Reserved
	CCI0, 8,	// UCSI DS CCI 0
	CCI1, 8,	// UCSI DS CCI 1
	CCI2, 8,	// UCSI DS CCI 2
	CCI3, 8,	// UCSI DS CCI 3
	CTL0, 8,	// UCSI DS Control 0
	CTL1, 8,	// UCSI DS Control 0
	CTL2, 8,	// UCSI DS Control 0
	CTL3, 8,	// UCSI DS Control 0
	CTL4, 8,	// UCSI DS Control 0
	CTL5, 8,	// UCSI DS Control 0
	CTL6, 8,	// UCSI DS Control 0
	CTL7, 8,	// UCSI DS Control 0

	Offset(0xf0),
	P0SD, 8,	// PD Port Status DD
	P0S4, 8,	// PD Port Status 4
	P0S5, 8,	// PD Port Status 5
	P0SE, 8,	// PD Port Status E
	P0SA, 8,	// PD Port Status 10
	P0SB, 8,	// PD Port Status 11

	Offset(0xfd),
	STCD, 8,	// Shutdown Code
	EJ8R, 8,	// EJ898A Need Reboot
	EJ8E, 8,	// EJ898A Error
}
