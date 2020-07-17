// From https://review.coreboot.org/c/coreboot/+/28380
/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017-2018 Patrick Rudolph <siro@das-labor.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Nvidia Optimus support methods.
 *
 * Methods defined here are known to work on Lenovo's Sandy Bridge
 * and Ivy Bridge series, which have GPIO21 pulled low on installed dGPU and
 * GPIO17 to detect dGPU "PowerGood". They use the same PMH7 functions to
 * enable dGPU power and handle dGPU reset.
 */

#define DGPU_RST_N GPP_F22
#define DGPU_PWR_EN GPP_F23
#define DGPU_GC6 GPP_K21

Device (\_SB.PCI0.PEGP.DEV0)
{
	Name(_ADR, 0x00000000)
	Name (_STA, 0xF)
	Name (LTRE, 0)

	// Memory mapped PCI express registers
	// Not sure what this stuff is, but it is used to get into GC6
	OperationRegion (RPCX, SystemMemory, 0xE0008000, 0x1000)
	Field (RPCX, ByteAcc, NoLock, Preserve)
	{
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

	Method (_ON)
	{
		Debug = "PEGP.DEV0._ON"

		If (_STA != 0xF) {
			Debug = "  If DGPU_PWR_EN low"
			If (! GTXS (DGPU_PWR_EN)) {
				Debug = "    DGPU_PWR_EN high"
				STXS (DGPU_PWR_EN)

				Debug = "    Sleep 16"
				Sleep (16)
			}

			Debug = "  DGPU_RST_N high"
			STXS(DGPU_RST_N)

			Debug = "  Sleep 10"
			Sleep (10)

			Debug = "  Q0L0 = 1"
			Q0L0 = 1

			Debug = "  Sleep 16"
			Sleep (16)

			Debug = "  While Q0L0"
			Local0 = 0
			While (Q0L0) {
				If ((Local0 > 4)) {
					Debug = "    While Q0L0 timeout"
					Break
				}

				Sleep (16)
				Local0++
			}

			Debug = "  P0RM = 0"
			P0RM = 0

			Debug = "  P0AP = 0"
			P0AP = 0

			Debug = Concatenate("  LREN = ", ToHexString(LTRE))
			LREN = LTRE

			Debug = "  CEDR = 1"
			CEDR = 1

			Debug = "  CMDR |= 7"
			CMDR |= 7

			Debug = "  _STA = 0xF"
			_STA = 0xF
		}
	}

	Method (_OFF)
	{
		Debug = "PEGP.DEV0._OFF"

		If (_STA != 0x5) {
			Debug = Concatenate("  LTRE = ", ToHexString(LREN))
			LTRE = LREN

			Debug = "  Q0L2 = 1"
			Q0L2 = 1

			Debug = "  Sleep 16"
			Sleep (16)

			Debug = "  While Q0L2"
			Local0 = Zero
			While (Q0L2) {
				If ((Local0 > 4)) {
					Debug = "    While Q0L2 timeout"
					Break
				}

				Sleep (16)
				Local0++
			}

			Debug = "  P0RM = 1"
			P0RM = 1

			Debug = "  P0AP = 3"
			P0AP = 3

			Debug = "  Sleep 10"
			Sleep (10)

			Debug = "  DGPU_RST_N low"
			CTXS(DGPU_RST_N)

			Debug = "  While DGPU_GC6 low"
			Local0 = Zero
			While (! GRXS(DGPU_GC6)) {
				If ((Local0 > 4)) {
					Debug = "    While DGPU_GC6 low timeout"

					Debug = "    DGPU_PWR_EN low"
					CTXS (DGPU_PWR_EN)
					Break
				}

				Sleep (16)
				Local0++
			}

			Debug = "  _STA = 0x5"
			_STA = 0x5
		}
	}
}
