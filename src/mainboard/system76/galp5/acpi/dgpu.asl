/* SPDX-License-Identifier: GPL-2.0-only */
External (\_SB.PCI0.RP01.RTD3)

Device (\_SB.PCI0.RP01.DEV0) {
	Name(_ADR, 0x00000000)

    Name (_PR0, Package() { \_SB.PCI0.RP01.RTD3 })
    Name (_PR3, Package() { \_SB.PCI0.RP01.RTD3 })

    Method (_PS0, 0) {
        Debug = "GPU _PS0"
        STXS (DGPU_EVENT_N)
        Sleep (1)
        CTXS (DGPU_EVENT_N)
        Sleep (1)
        STXS (DGPU_EVENT_N)
    }

    Method (_PS3, 0) {
        Debug = "GPU _PS3"
    }
}
