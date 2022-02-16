/* SPDX-License-Identifier: GPL-2.0-only */

Name (LTRE, 0)

// Memory mapped PCI express registers
// Not sure what this stuff is, but it is used to get into GC6
// TODO: use GPU config to generate address
OperationRegion (RPCX, SystemMemory, CONFIG_ECAM_MMCONF_BASE_ADDRESS + 0x8000, 0x1000)
Field (RPCX, ByteAcc, NoLock, Preserve) {
	PVID,   16,
	PDID,   16,
	CMDR,   8,
	Offset (0x19),
	PRBN,   8,
	Offset (0x84),
	D0ST,   2,
	Offset (0xAA),
	CEDR,   1,
	Offset (0xAC),
		,   4,
	CMLW,   6,
	Offset (0xB0),
	ASPM,   2,
		,   2,
	P0LD,   1,
	RTLK,   1,
	Offset (0xC9),
		,   2,
	LREN,   1,
	Offset (0x11A),
		,   1,
	VCNP,   1,
	Offset (0x214),
	Offset (0x216),
	P0LS,   4,
	Offset (0x248),
		,   7,
	Q0L2,   1,
	Q0L0,   1,
	Offset (0x504),
	Offset (0x506),
	PCFG,   2,
	Offset (0x508),
	TREN,   1,
	Offset (0xC20),
		,   4,
	P0AP,   2,
	Offset (0xC38),
		,   3,
	P0RM,   1,
	Offset (0xC74),
	P0LT,   4,
	Offset (0xD0C),
		,   20,
	LREV,   1
}

// Enter L23
Method (DL23, 0, Serialized) {
	Printf("      GPU PORT DL23 START")

	Debug = Concatenate("        LTRE = ", ToHexString(LREN))
	LTRE = LREN

	Debug = "        Q0L2 = 1"
	Q0L2 = 1

	Debug = "        Sleep 16"
	Sleep (16)

	Debug = "        While Q0L2"
	Local0 = Zero
	While (Q0L2) {
		If ((Local0 > 4)) {
			Debug = "          While Q0L2 timeout"
			Break
		}

		Sleep (16)
		Local0++
	}

	Debug = "        P0RM = 1"
	P0RM = 1

	Debug = "        P0AP = 3"
	P0AP = 3

	Printf("      GPU PORT DL23 FINISH")
}

// Exit L23
Method (L23D, 0, Serialized) {
	Printf("      GPU PORT L23D START")

	Debug = "        Q0L0 = 1"
	Q0L0 = 1

	Debug = "        Sleep 16"
	Sleep (16)

	Debug = "        While Q0L0"
	Local0 = 0
	While (Q0L0) {
		If ((Local0 > 4)) {
			Debug = "          While Q0L0 timeout"
			Break
		}

		Sleep (16)
		Local0++
	}

	Debug = "        P0RM = 0"
	P0RM = 0

	Debug = "        P0AP = 0"
	P0AP = 0

	Debug = Concatenate("        LREN = ", ToHexString(LTRE))
	LREN = LTRE

	Debug = "        CEDR = 1"
	CEDR = 1

	Debug = "        CMDR |= 7"
	CMDR |= 7

	Printf("      GPU PORT L23D FINISH")
}


// Main power resource
PowerResource (PWRR, 0, 0) {
	Name (_STA, 1)

	Method (_ON, 0, Serialized) {
		Printf("GPU PORT PWRR._ON")

		^^DEV0._ON()

		_STA = 1
	}

	Method (_OFF, 0, Serialized) {
		Printf("GPU PORT PWRR._OFF")

		^^DEV0._OFF()

		_STA = 0
	}
}

// Power resources for entering D0
Name (_PR0, Package () { PWRR })

// Power resources for entering D3
Name (_PR3, Package () { PWRR })
