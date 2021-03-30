/* SPDX-License-Identifier: GPL-2.0-only */

Device (\_SB.PCI0.PEGP) {
	Name (_ADR, CONFIG_DRIVERS_SYSTEM76_DGPU_DEVICE << 16)

// Power state {
	Name (_PSC, 0)

	Method (_PS0) {
		Printf("PEGP _PS0 {")
		_PSC = 0
		Printf("} PEGP _PS0")
	}

	Method (_PS3) {
		Printf("PEGP _PS3 {")
		_PSC = 3
		Printf("} PEGP _PS3")
	}

	PowerResource (PWRR, 0, 0) {
		Name (_STA, 1)

		Method (_ON) {
			Printf("PEGP.PWRR._ON {")
			If (_STA != 1) {
				\_SB.PCI0.PEGP.DEV0._ON ()
			}
			_STA = 1
			Printf("} PEGP.PWRR._ON")
		}

		Method (_OFF) {
			Printf("PEGP.PWRR._OFF {")
			If (_STA != 0) {
				\_SB.PCI0.PEGP.DEV0._OFF ()
			}
			_STA = 0
			Printf("} PEGP.PWRR._OFF")
		}
	}

	Name (_PR0, Package () { \_SB.PCI0.PEGP.PWRR })
	Name (_PR2, Package () { \_SB.PCI0.PEGP.PWRR })
	Name (_PR3, Package () { \_SB.PCI0.PEGP.PWRR })
// } Power state
}

Device (\_SB.PCI0.PEGP.DEV0) {
	Name(_ADR, 0x00000000)
	Name (_STA, 0xF)

// Power state {
	Name (_PSC, 0)

	Method (_PS0) {
		Printf("NVIDIA _PS0 {")
		_PSC = 0
		Printf("} NVIDIA _PS0")
	}

	Method (_PS3) {
		Printf("NVIDIA _PS3 {")
		_PSC = 3
		Printf("} NVIDIA _PS3")
	}

    OperationRegion (PCIC, PCI_Config, 0x00, 0x50)
    Field (PCIC, DwordAcc, NoLock, Preserve) {
        Offset (0x40),
        SSID, 32
    }

	PowerResource (PWRR, 0, 0) {
		Name (_STA, 1)

		Method (_ON) {
			Printf("NVIDIA PWRR._ON {")
			^^SSID = 0x65E51558
			Printf("  SSID %o", ToHexString(^^SSID))
			_STA = 1
			Printf("} NVIDIA PWRR._ON")
		}

		Method (_OFF) {
			Printf("NVIDIA PWRR._OFF {")
			Printf("  SSID %o", ToHexString(^^SSID))
			_STA = 0
			Printf("} NVIDIA PWRR._OFF")
		}
	}

	Name (_PR0, Package () { PWRR })
	Name (_PR2, Package () { PWRR })
	Name (_PR3, Package () { PWRR })
// } Power state

	Name (LTRE, 0)

	// Memory mapped PCI express registers
	// Not sure what this stuff is, but it is used to get into GC6
        //TODO: use DGPU_DEVICE to generate address
	OperationRegion (RPCX, SystemMemory, CONFIG_MMCONF_BASE_ADDRESS + 0x8000, 0x1000)
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

	// Power on/off endpoint
	Method (GPPR, 1) {
		If (Arg0 == 0) {
			Printf("GPPR OFF {")
			CTXS(DGPU_RST_N)
			Sleep(2) // DLHR - delay after hold reset
			CTXS(DGPU_PWR_EN)
			Sleep(75) // NGDT
			Printf("} GPPR OFF")
		} ElseIf (Arg0 == 1) {
			Printf("GPPR ON {")
			STXS(DGPU_PWR_EN)
			Sleep(7) // DLPW - delay after power
			STXS(DGPU_RST_N)
			Sleep(2) // DLHR - delay after hold reset
			Printf("} GPPR ON")
		}
	}

	// Enable link for RTD3
	Method (RTEN) {
		Printf("RTEN {")
		Q0L0 = 1
		Sleep(16)

		Local0 = 0
		While (Q0L0) {
			If (Local0 > 4) {
				Break
			}

			Sleep(16)
			Local0++
		}

		P0RM = 0
		P0AP = 0
		Printf("} RTEN")
	}

	// Disable link for RTD3
	Method (RTDS) {
		Printf("RTDS {")
		Q0L2 = 1
		Sleep(16)

		Local0 = 0
		While (Q0L2) {
			If (Local0 > 4) {
				Break
			}

			Sleep (16)
			Local0++
		}

		P0RM = 1
		P0AP = 3
		Printf("} RTDS")
	}

	Method (_ON) {
		Printf("PEGP.DEV0._ON {")
		If (_STA != 0xF) {
			// Enable source clock
			PCRA(0xDC, 0x100C, ~0x0100)
			Sleep(16)

			// Power on endpoint
			GPPR(1)

			// RTD3 link enable
			RTEN()

			//TODO: wait for P0LS

			// Restore link state?
			LREN = LTRE
			CEDR = 1
			CMDR |= 7
		}
		_STA = 0xF
		Printf("} PEGP.DEV0._ON")
	}

	Method (_OFF) {
		Printf("PEGP.DEV0._OFF {")
		If (_STA != 0x5) {
			// Save link state
			LTRE = LREN

			// RTD3 link disable
			RTDS()

			// Disable source clock
			PCRO(0xDC, 0x100C, 0x0100)
			Sleep(16)

			// Power off endpoint
			GPPR(0)
		}
		_STA = 0x5
		Printf("} PEGP.DEV0._OFF")
	}
}
