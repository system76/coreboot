// From https://review.coreboot.org/c/coreboot/+/40625
/* SPDX-License-Identifier: GPL-2.0-only */

Device (PEGP)
{
	Name (_ADR, 0x00010000)

	PowerResource (PWRR, 0, 0)
	{
		Name (_STA, 1)

		Method (_ON)
		{
			Debug = "PEGP.PWRR._ON"
			If (_STA != 1) {
				\_SB.PCI0.PEGP.DEV0._ON ()
				_STA = 1
			}
		}

		Method (_OFF)
		{
			Debug = "PEGP.PWRR._OFF"
			If (_STA != 0) {
				\_SB.PCI0.PEGP.DEV0._OFF ()
				_STA = 0
			}
		}
	}

	Name (_PR0, Package () { \_SB.PCI0.PEGP.PWRR })
	Name (_PR2, Package () { \_SB.PCI0.PEGP.PWRR })
	Name (_PR3, Package () { \_SB.PCI0.PEGP.PWRR })
}
