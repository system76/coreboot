/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB.PCI0.RP01) {
    Device (DEV0) {
        Name(_ADR, 0x00000000)

        OperationRegion (PCIC, PCI_Config, 0x00, 0x50)
        Field (PCIC, DwordAcc, NoLock, Preserve) {
            Offset (0x40),
            SSID, 32
        }

        Name (_PR0, Package () { PWRR })
        Name (_PR3, Package () { PWRR })
        PowerResource (PWRR, 0, 0) {
            Name (_STA, 1)

            Method (_ON) {
                ^^SSID = 0x50171558
                Printf("GPU _ON %o", ToHexString(^^SSID))
                _STA = 1
            }

            Method (_OFF) {
                Printf("GPU _OFF %o", ToHexString(^^SSID))
                _STA = 0
            }
        }
    }
}
