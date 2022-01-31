/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB.PCI0.I2C1)
{
	Device (STSA)
	{
		Name (_HID, "SYTS7508")
		Name (_CID, "PNP0C50")
		Name (_UID, 4)
		Name (ISTP, 0) /* TouchScreen */

		/* Fetch HidDescriptorAddress, Register offset in the
		 * I2C device at which the HID descriptor can be read
		 */
		Method (_DSM, 4, NotSerialized)
		{
			If (LEqual (Arg0, ToUUID (
				"3cdff6f7-4267-4555-ad05-b30a3d8938de")))
			{
				// DSM Revision
				If (LEqual (Arg2, Zero))
				{
					If (LEqual (Arg1, One))
					{
						Return (Buffer (One)
						{
							0x03
						})
					}
					Else
					{
						Return (Buffer (One)
						{
							0x00
						})
					}
				}
				// HID Function
				If (LEqual (Arg2, One))
				{
					Return (0x20)
				}
			}
			Else
			{
				Return (Buffer (One)
				{
					0x00
				})
			}

			Return (Zero)
		}

		Method(_CRS, 0x0, NotSerialized)
		{
			Name (BUF0, ResourceTemplate ()
			{
				I2cSerialBus(
					0x20,                     /* SlaveAddress */
					ControllerInitiated,      /* SlaveMode */
					400000,                   /* ConnectionSpeed */
					AddressingMode7Bit,       /* AddressingMode */
					"\\_SB.PCI0.I2C1",             /* ResourceSource */
				)
				GpioInt (Level, ActiveLow, SharedAndWake, PullDefault,,
					"\\_SB.GPNC") { BOARD_TOUCH_GPIO_INDEX }
			})
			Return (BUF0)
		}

		Method (_STA)
		{
			If (LEqual (\S1EN, 1)) {
				Return (0xF)
			} Else {
				Return (0x0)
			}
		}

		Name (_PRW, Package() { BOARD_TOUCHSCREEN_WAKE_GPIO, 0x3 })

		/* Allow device to power off in S0 */
		Name (_S0W, 4)
	}
}
