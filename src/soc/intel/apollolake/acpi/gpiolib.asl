/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB)
{
	/* Get Pad Configuration DW0 register value */
	Method (GPC0, 0x1, Serialized)
	{
		/* Arg0 - GPIO DW0 address */
		Store (Arg0, Local0)
		OperationRegion (PDW0, SystemMemory, Local0, 4)
		Field (PDW0, AnyAcc, NoLock, Preserve) {
			TEMP, 32
		}
		Return (TEMP)
	}

	/* Set Pad Configuration DW0 register value */
	Method (SPC0, 0x2, Serialized)
	{
		/* Arg0 - GPIO DW0 address */
		/* Arg1 - Value for DW0 register */
		Store (Arg0, Local0)
		OperationRegion (PDW0, SystemMemory, Local0, 4)
		Field (PDW0, AnyAcc, NoLock, Preserve) {
			TEMP,32
		}
		Store (Arg1, TEMP)
	}

	/* Get Pad Configuration DW1 register value */
	Method (GPC1, 0x1, Serialized)
	{
		/* Arg0 - GPIO DW0 address */
		Store (Arg0 + 4, Local0)
		OperationRegion (PDW1, SystemMemory, Local0, 4)
		Field (PDW1, AnyAcc, NoLock, Preserve) {
			TEMP, 32
		}
		Return (TEMP)
	}

	/* Set Pad Configuration DW1 register value */
	Method (SPC1, 0x2, Serialized)
	{
		/* Arg0 - GPIO DW0 address */
		/* Arg1 - Value for DW1 register */
		Store (Arg0 + 4, Local0)
		OperationRegion (PDW1, SystemMemory, Local0, 4)
		Field(PDW1, AnyAcc, NoLock, Preserve) {
			TEMP,32
		}
		Store (Arg1, TEMP)
	}

	/* Get DW0 address of a given pad */
	Method (GDW0, 0x2, Serialized)
	{
		/* Arg0 - GPIO portid */
		/* Arg1 - GPIO pad offset relative to the community */
		Store (0, Local1)
		Or( Or (ShiftLeft (Arg0, 16), CONFIG_PCR_BASE_ADDRESS),
					Local1, Local1)
		Or(PAD_CFG_BASE + Arg1 * GPIO_NUM_PAD_CFG_REGS * 4, Local1, Local1)
		Return (Local1)
	}

	/* Calculate HOSTSW_REG address */
	Method (CHSA, 0x1, Serialized)
	{
		/* Arg0 - GPIO pad offset relative to the community */
		Local1 = HOSTSW_OWN_REG_0 + Arg0 / 32 * 4
		Return (Local1)
	}

	/* Get Host ownership register of GPIO Community */
	Method (GHO, 0x2, Serialized)
	{
		/* Arg0 - GPIO portid */
		/* Arg1 - GPIO pad offset relative to the community */
		Store (CHSA (Arg1), Local1)

		OperationRegion (SHO0, SystemMemory, Or ( Or
			(CONFIG_PCR_BASE_ADDRESS, ShiftLeft (Arg0, 16)), Local1), 4)
		Field (SHO0, AnyAcc, NoLock, Preserve) {
			TEMP, 32
		}
		Return (TEMP)
	}

	/* Set Host ownership register of GPIO Community */
	Method (SHO, 0x3, Serialized)
	{
		/* Arg0 - GPIO portid */
		/* Arg1 - GPIO pad offset relative to the community */
		/* Arg2 - Value for Host own register */
		Store (CHSA (Arg1), Local1)

		OperationRegion (SHO0, SystemMemory, Or ( Or
			(CONFIG_PCR_BASE_ADDRESS, ShiftLeft (Arg0, 16)), Local1), 4)
		Field (SHO0, AnyAcc, NoLock, Preserve) {
			TEMP, 32
		}
		Store (Arg2, TEMP)
	}
}
