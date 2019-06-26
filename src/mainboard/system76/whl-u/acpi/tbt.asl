/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 System76
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

// See https://docs.microsoft.com/en-us/windows-hardware/drivers/pci/dsd-for-pcie-root-ports
Scope(\_SB.PCI0.RP05) {
    Method(_DSD, 0, NotSerialized) {
        Return (Package(4) {
            // https://docs.microsoft.com/en-us/windows-hardware/drivers/pci/dsd-for-pcie-root-ports#identifying-pcie-root-ports-supporting-hot-plug-in-d3
            ToUUID("6211e2c0-58a3-4af3-90e1-927a4e0c55a4"),
            Package(1) {
                Package(2) {
                    "HotPlugSupportInD3",
                    1
                }
            },
            // https://docs.microsoft.com/en-us/windows-hardware/drivers/pci/dsd-for-pcie-root-ports#identifying-externally-exposed-pcie-root-ports
            ToUUID("efcc06cc-73ac-4bc3-bff0-76143807c389"),
            Package(2) {
                Package(2) {
                    "ExternalFacingPort",
                    1
                },
                Package(2) {
                    "UID",
                    0
                }
            }
        })
    }
}
