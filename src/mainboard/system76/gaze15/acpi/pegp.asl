// From https://review.coreboot.org/c/coreboot/+/40625
/* SPDX-License-Identifier: GPL-2.0-only */

Device (PEGP)
{
	Name (_ADR, 0x00010000)
	Name (_STA, 0xF)

	PowerResource (PWRR, 0, 0)
	{
		Name (_STA, 1)

		Method (_ON)
		{
			Debug = "PEGP.PWRR._ON"
			If (_STA != 1) {
				\_SB.PCI0.PEGP._ON ()
				_STA = 1
			}
		}

		Method (_OFF)
		{
			Debug = "PEGP.PWRR._OFF"
			If (_STA != 0) {
				\_SB.PCI0.PEGP._OFF ()
				_STA = 0
			}
		}
	}

	/* Depend on the CLK to be active. _PR3 is also searched by nouveau to
	 * detect "Windows 8 compatible Optimus _DSM handler".
	 */
	Name (_PRE, Package () { \_SB.PCI0.PEGP.PWRR })
	Name (_PR0, Package () { \_SB.PCI0.PEGP.PWRR })
	Name (_PR3, Package () { \_SB.PCI0.PEGP.PWRR })

	Name (_PSC, 0)

	Method (_PS0)
	{
		Debug = "PEGP._PS0"
		_PSC = 0
	}

	Method (_PS3)
	{
		Debug = "PEGP._PS3"
		_PSC = 3
	}

	Method (_ON)
	{
		Debug = "PEGP._ON"
		If (_STA != 0xF)
		{
			^DEV0._ON ()
			_STA = 0xF
		}
	}

	Method (_OFF)
	{
		Debug = "PEGP._OFF"
		If (_STA != 0x5)
		{
			^DEV0._OFF ()
			_STA = 0x5
		}
	}
}
