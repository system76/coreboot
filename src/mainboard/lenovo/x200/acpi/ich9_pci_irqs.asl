/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

/* This is board specific information: IRQ routing for the
 * 0:1e.0 PCI bridge of the ICH9
 */

/* TODO: which slots are actually relevant? */
If (PICM) {
	Return (Package() {
		// PCI Slot 1 routes ABCD
		Package() { 0x0000ffff, 0, 0, 16},
		Package() { 0x0000ffff, 1, 0, 17},
		Package() { 0x0000ffff, 2, 0, 18},
		Package() { 0x0000ffff, 3, 0, 19},

		// PCI Slot 2 routes BCDA
		Package() { 0x0001ffff, 0, 0, 17},
		Package() { 0x0001ffff, 1, 0, 18},
		Package() { 0x0001ffff, 2, 0, 19},
		Package() { 0x0001ffff, 3, 0, 16},

		// PCI Slot 3 routes CDAB
		Package() { 0x0002ffff, 0, 0, 18},
		Package() { 0x0002ffff, 1, 0, 19},
		Package() { 0x0002ffff, 2, 0, 16},
		Package() { 0x0002ffff, 3, 0, 17},

		// PCI Slot 4 routes ABCD
		Package() { 0x0003ffff, 0, 0, 16},
		Package() { 0x0003ffff, 1, 0, 17},
		Package() { 0x0003ffff, 2, 0, 18},
		Package() { 0x0003ffff, 3, 0, 19},

		// PCI Slot 5 routes ABCD
		Package() { 0x0004ffff, 0, 0, 16},
		Package() { 0x0004ffff, 1, 0, 17},
		Package() { 0x0004ffff, 2, 0, 18},
		Package() { 0x0004ffff, 3, 0, 19},

		// PCI Slot 6 routes BCDA
		Package() { 0x0005ffff, 0, 0, 17},
		Package() { 0x0005ffff, 1, 0, 18},
		Package() { 0x0005ffff, 2, 0, 19},
		Package() { 0x0005ffff, 3, 0, 16},

		// FIXME: what's this supposed to mean? (adopted from ich7)
		//Package() { 0x0008ffff, 0, 0, 20},
	})
} Else {
	Return (Package() {
		// PCI Slot 1 routes ABCD
		Package() { 0x0000ffff, 0, \_SB.PCI0.LPCB.LNKA, 0},
		Package() { 0x0000ffff, 1, \_SB.PCI0.LPCB.LNKB, 0},
		Package() { 0x0000ffff, 2, \_SB.PCI0.LPCB.LNKC, 0},
		Package() { 0x0000ffff, 3, \_SB.PCI0.LPCB.LNKD, 0},

		// PCI Slot 2 routes BCDA
		Package() { 0x0001ffff, 0, \_SB.PCI0.LPCB.LNKB, 0},
		Package() { 0x0001ffff, 1, \_SB.PCI0.LPCB.LNKC, 0},
		Package() { 0x0001ffff, 2, \_SB.PCI0.LPCB.LNKD, 0},
		Package() { 0x0001ffff, 3, \_SB.PCI0.LPCB.LNKA, 0},

		// PCI Slot 3 routes CDAB
		Package() { 0x0002ffff, 0, \_SB.PCI0.LPCB.LNKC, 0},
		Package() { 0x0002ffff, 1, \_SB.PCI0.LPCB.LNKD, 0},
		Package() { 0x0002ffff, 2, \_SB.PCI0.LPCB.LNKA, 0},
		Package() { 0x0002ffff, 3, \_SB.PCI0.LPCB.LNKB, 0},

		// PCI Slot 4 routes ABCD
		Package() { 0x0003ffff, 0, \_SB.PCI0.LPCB.LNKA, 0},
		Package() { 0x0003ffff, 1, \_SB.PCI0.LPCB.LNKB, 0},
		Package() { 0x0003ffff, 2, \_SB.PCI0.LPCB.LNKC, 0},
		Package() { 0x0003ffff, 3, \_SB.PCI0.LPCB.LNKD, 0},

		// PCI Slot 5 routes ABCD
		Package() { 0x0004ffff, 0, \_SB.PCI0.LPCB.LNKA, 0},
		Package() { 0x0004ffff, 1, \_SB.PCI0.LPCB.LNKB, 0},
		Package() { 0x0004ffff, 2, \_SB.PCI0.LPCB.LNKC, 0},
		Package() { 0x0004ffff, 3, \_SB.PCI0.LPCB.LNKD, 0},

		// PCI Slot 6 routes BCDA
		Package() { 0x0005ffff, 0, \_SB.PCI0.LPCB.LNKB, 0},
		Package() { 0x0005ffff, 1, \_SB.PCI0.LPCB.LNKC, 0},
		Package() { 0x0005ffff, 2, \_SB.PCI0.LPCB.LNKD, 0},
		Package() { 0x0005ffff, 3, \_SB.PCI0.LPCB.LNKA, 0},

		// FIXME
		// Package() { 0x0008ffff, 0, \_SB.PCI0.LPCB.LNKE, 0},
	})
}
