/* SPDX-License-Identifier: GPL-2.0-only */

/* Method called from _PTS prior to enter sleep state */
Method (MPTS, 1) {
	\_SB.SIO.PTS()
}

/* Method called from _WAK prior to wakeup */
Method (MWAK, 1) {
	\_SB.SIO.WAK()
}
