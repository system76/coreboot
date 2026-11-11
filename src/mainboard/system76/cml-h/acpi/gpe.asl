/* SPDX-License-Identifier: GPL-2.0-only */

#if CONFIG(BOARD_SYSTEM76_ORYP5)
// GPP_B23 SCI
Method (_L17, 0, Serialized)
{
	Printf ("GPE _L17: %o", ToHexString(\_SB.PCI0.LPCB.EC0.WFNO))
	If (\_SB.PCI0.LPCB.EC0.ECOK) {
		If (\_SB.PCI0.LPCB.EC0.WFNO == 1) {
			Notify(\_SB.LID0, 0x80)
		}
	}
}
#else
// GPP_K6 SCI
Method (_L06, 0, Serialized)
{
	Printf ("GPE _L06: %o", ToHexString(\_SB.PCI0.LPCB.EC0.WFNO))
	If (\_SB.PCI0.LPCB.EC0.ECOK) {
		If (\_SB.PCI0.LPCB.EC0.WFNO == 1) {
			Notify(\_SB.LID0, 0x80)
		}
	}
}
#endif
