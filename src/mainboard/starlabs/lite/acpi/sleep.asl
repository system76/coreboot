/* SPDX-License-Identifier: GPL-2.0-only */

Method (MPTS, 1, NotSerialized)
{
	If (Arg0)
	{
		RPTS (Arg0)
	}
}

Method (MWAK, 1, NotSerialized)
{
	RWAK (Arg0)
	Return (0x00)
}
