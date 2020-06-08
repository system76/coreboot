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

Scope (\_SB.PCI0.PEGP.DEV0)
{
	Method (_ON)
	{
		Debug = "PEGP.DEV0._ON"
		If (! GTXS (DGPU_PWR_EN)) {
			Debug = "DGPU reset start"
			CTXS (DGPU_RST_N)
			Debug = "DGPU power enable"
			STXS (DGPU_PWR_EN)
			Sleep (4)
			Debug = "DGPU reset finish"
			STXS (DGPU_RST_N)
			Sleep (50)
		}
	}

	Method (_OFF)
	{
		Debug = "PEGP.DEV0._OFF"
		If (GTXS (DGPU_PWR_EN)) {
			Debug = "DGPU reset start"
			CTXS (DGPU_RST_N)
			Sleep (4)
			Debug = "DGPU power disable"
			CTXS (DGPU_PWR_EN)
			Sleep (50)
		}
	}

	Method (_STA)
	{
		If (GTXS (DGPU_PWR_EN)) {
			Debug = "PEGP.DEV0._STA F"
			Return (0xF)
		} Else {
			Debug = "PEGP.DEV0._STA 5"
			Return (0x5)
		}
	}
}
