/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Method called from _PTS prior to system sleep state entry
 * Enables dynamic clock gating for all 5 GPIO communities
 */
Method (MPTS, 1, Serialized)
{
	\_SB.PCI0.LPCB.EC0.PTS (Arg0)
}

/*
 * Method called from _WAK prior to system sleep state wakeup
 * Disables dynamic clock gating for all 5 GPIO communities
 */
Method (MWAK, 1, Serialized)
{
	\_SB.PCI0.LPCB.EC0.WAK (Arg0)
}
