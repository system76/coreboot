/* SPDX-License-Identifier: GPL-2.0-only */

Device (\_SB.PCI0.PEGP) {
	Name (_ADR, CONFIG_DRIVERS_SYSTEM76_DGPU_DEVICE << 16)

// Power state {
	Name (_PSC, 0)

	// Power state needs to power on
	Name (PSPO, 0)

	Method (_PS0) {
		Printf("NVIDIA _PS0 {")
		If (PSPO) {
			^DEV0._ON()
			PSPO = 0
		}
		_PSC = 0
		Printf("} NVIDIA _PS0")
	}

	Method (_PS3) {
		Printf("NVIDIA _PS3 {")
		If (^DEV0.OPPW == 3) {
			^DEV0._OFF()
			^DEV0.OPPW = 2
			PSPO = 1
		}
		_PSC = 3
		Printf("} NVIDIA _PS3")
	}

	PowerResource (PWRR, 0, 0) {
		Name (_STA, 1)

		Method (_ON) {
			Printf("NVIDIA PWRR._ON {")
			If (_STA != 1) {
				If (^^DEV0.DPCX) {
					^^DEV0.NVPC(^^DEV0.DPCX)
					^^DEV0.DPCX = 0
				} Else {
					^^DEV0._ON()
				}
				_STA = 1
			}
			Printf("} NVIDIA PWRR._ON")
		}

		Method (_OFF) {
			Printf("NVIDIA PWRR._OFF {")
			If (_STA != 0) {
				If (^^DEV0.DPC) {
					^^DEV0.NVPC(^^DEV0.DPC)
					^^DEV0.DPC = 0
				} Else {
					^^DEV0._OFF()
				}
				_STA = 0
			}
			Printf("} NVIDIA PWRR._OFF")
		}
	}

	Name (_PR0, Package () { PWRR })
	Name (_PR2, Package () { PWRR })
	Name (_PR3, Package () { PWRR })
// }
}

Device (\_SB.PCI0.PEGP.DEV0) {
	Name(_ADR, 0x00000000)
	Name (_STA, 0xF)

// DEBUGGING {
	// Convert a byte to a hex string, trimming extra parts
	Method (BHEX, 1) {
		Local0 = ToHexString(Arg0)
		Return (Mid(Local0, SizeOf(Local0) - 2, 2))
	}

	// UUID to string
	Method (IDST, 1) {
		Local0 = ""
		Fprintf(
			Local0,
			"%o%o%o%o-%o%o-%o%o-%o%o-%o%o%o%o%o%o",
			BHEX(DerefOf(Arg0[3])),
			BHEX(DerefOf(Arg0[2])),
			BHEX(DerefOf(Arg0[1])),
			BHEX(DerefOf(Arg0[0])),
			BHEX(DerefOf(Arg0[5])),
			BHEX(DerefOf(Arg0[4])),
			BHEX(DerefOf(Arg0[7])),
			BHEX(DerefOf(Arg0[6])),
			BHEX(DerefOf(Arg0[8])),
			BHEX(DerefOf(Arg0[9])),
			BHEX(DerefOf(Arg0[10])),
			BHEX(DerefOf(Arg0[11])),
			BHEX(DerefOf(Arg0[12])),
			BHEX(DerefOf(Arg0[13])),
			BHEX(DerefOf(Arg0[14])),
			BHEX(DerefOf(Arg0[15]))
		)
		Return (Local0)
	}

	// Safe hex conversion, checks type first
	Method (SFST, 1) {
		Local0 = ObjectType(Arg0)
		If (Local0 == 1 || Local0 == 2 || Local0 == 3) {
			Return (ToHexString(Arg0))
		} Else {
			Return (Concatenate("Type: ", Arg0))
		}
	}
// } DEBUGGING

// NVIDIA DSMs {
	#define NVIDIA_ERROR_UNSPECIFIED 0x80000001
	#define NVIDIA_ERROR_UNSUPPORTED 0x80000002

	#define NBCI_DSM_GUID "D4A50B75-65C7-46F7-BFB7-41514CEA0244"
	#define NBCI_REVISION_ID 0x0102

	#define NBCI_FUNC_SUPPORT 0
	#define NBCI_FUNC_GETOBJBYTYPE 16

	Method (NBCI, 2, Serialized) {
		Printf("NBCI {")
		Local0 = NVIDIA_ERROR_UNSUPPORTED

		Switch (ToInteger(Arg0)) {
			Case (NBCI_FUNC_SUPPORT) {
				Printf("  NBCI_FUNC_SUPPORT")

				Local0 = Buffer (4) { 0, 0, 0, 0 }

				CreateField(Local0, NBCI_FUNC_SUPPORT, 1, FNSP)
				FNSP = 1

				CreateField(Local0, NBCI_FUNC_GETOBJBYTYPE, 1, FNGO)
				FNGO = 1
			}
			Case (NBCI_FUNC_GETOBJBYTYPE) {
				Printf("  NBCI_FUNC_GETOBJBYTYPE")

				// Object type
				CreateField(Arg1, 16, 16, OBJT)
				Switch (ToInteger(OBJT)) {
					// 'DR' - driver object
					Case (0x4452) {
						//TODO: Per board data, this is for addw1
						Printf("  Return driver object")
						Printf("} NBCI")
						Return (Buffer(0xA1) {
							0x57, 0x74, 0xDC, 0x86, 0x75, 0x84, 0xEC, 0xE7,
							0x52, 0x44, 0xA1, 0x00, 0x00, 0x00, 0x00, 0x01,
							0x00, 0x00, 0x00, 0x00, 0xDE, 0x10, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x34, 0x00,
							0x00, 0x00, 0x01, 0x00, 0x47, 0x00, 0x00, 0x00,
							0x02, 0x00, 0x45, 0x00, 0x00, 0x00, 0x03, 0x00,
							0x51, 0x00, 0x00, 0x00, 0x04, 0x00, 0x4F, 0x00,
							0x00, 0x00, 0x05, 0x00, 0x4D, 0x00, 0x00, 0x00,
							0x06, 0x00, 0x4B, 0x00, 0x00, 0x00, 0x07, 0x00,
							0x49, 0x00, 0x00, 0x00, 0x08, 0x00, 0x47, 0x00,
							0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0xD9, 0x1C,
							0x04, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
							0x41, 0x5D, 0xC9, 0x00, 0x01, 0x24, 0x2E, 0x00,
							0x02, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x01,
							0x00, 0x00, 0x00, 0xD9, 0x1C, 0x04, 0x00, 0x00,
							0x00, 0x01, 0x00, 0x00, 0x00, 0x60, 0x68, 0x9E,
							0x35, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00
						})
					}
					// 'VK' - validation key
					Case (0x564B) {
						//TODO: Per board data, this is for addw1
						Printf("  Return validation key")
						Printf("} NBCI")
						Return (Buffer (0xf0) {
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
						})
					}
					Default {
						Printf("  Unknown object type: %o", SFST(OBJT))
					}
				}
			}
			Default {
				Printf("  Unsupported NBCI_FUNC: %o, %o", SFST(Arg0), SFST(Arg1))
			}
		}

		Printf("  Return %o", SFST(Local0))
		Printf("} NBCI")
		Return (Local0)
	}

	#define JT_DSM_GUID "CBECA351-067B-4924-9CBD-B46B00B86F34"
	#define JT_REVISION_ID 0x0200

	#define JT_FUNC_SUPPORT 0
	#define JT_FUNC_CAPS 1
	#define JT_FUNC_POWERCONTROL 3
	#define JT_FUNC_PLATPOLICY 4

	#define JT_GPC_GSS 0 // Get current GCx status
	#define JT_GPC_EGNS 1 // Enter GC6 only
	#define JT_GPC_EGIS 2 // Enter GC6 with panel self-refresh and SMI trap
	#define JT_GPC_XGXS 3 // Exit GC6, panel self-refresh, and SMI trap
	#define JT_GPC_XGIS 4 // Exit GC6 for self-refresh update
	#define JT_GPC_EGIN 5 // Enter GC6 with panel self-refresh
	#define JT_GPC_XCLM 6 // Trigger GPU_EVENT only

	// Deferred power control
	Name (DPC, 0)
	// Deferred power control on exit
	Name (DPCX, 0)

	Method (NVPC, 1, Serialized) {
		Printf("NVPC {")

		Switch (ToInteger(Arg0)) {
			Case (JT_GPC_GSS) {
				Printf("  JT_GPC_GSS")
			}
			Case (JT_GPC_EGNS) {
				Printf("  JT_GPC_EGNS")
				GC6I()
			}
			Case (JT_GPC_EGIS) {
				Printf("  JT_GPC_EGIS")
				GC6I()
			}
			Case (JT_GPC_XGXS) {
				Printf("  JT_GPC_XGXS")
				GC6O()
			}
			Case (JT_GPC_XGIS) {
				Printf("  JT_GPC_XGIS")
				GC6O()
			}
			Case (JT_GPC_EGIN) {
				Printf("  JT_GPC_EGIN")
				GC6I()
			}
			Case (JT_GPC_XCLM) {
				Printf("  JT_GPC_XCLM")
				//TODO: not implemented?
			}
			Default {
				Printf("  Unknown JT_GPC: %o", SFST(Arg0))
			}
		}

		Printf("} NVPC")
	}

	Method (NVJT, 2, Serialized) {
		Printf("NVJT {")
		Local0 = NVIDIA_ERROR_UNSUPPORTED

		Switch (ToInteger(Arg0)) {
			Case (JT_FUNC_SUPPORT) {
				Printf("  JT_FUNC_SUPPORT")

				Local0 = Buffer (4) { 0, 0, 0, 0 }

				CreateField(Local0, JT_FUNC_SUPPORT, 1, FNSP)
				FNSP = 1

				CreateField(Local0, JT_FUNC_CAPS, 1, FNCP)
				FNCP = 1

				CreateField(Local0, JT_FUNC_POWERCONTROL, 1, FNPC)
				FNPC = 1

				CreateField(Local0, JT_FUNC_PLATPOLICY, 1, FNPP)
				FNPP = 1
			}
			Case (JT_FUNC_CAPS) {
				Printf("  JT_FUNC_CAPS")

				Local0 = Buffer (4) { 0, 0, 0, 0 }

				// G-Sync NVSR power features enabled
				CreateField(Local0, 0, 1, JTE)
				JTE = 0

				// NVSR enabled
				CreateField(Local0, 1, 2, NVSE)
				//TODO: disabled by default until we figure out what to do
				NVSE = 1

				// Panel power rail
				CreateField(Local0, 3, 2, PPR)
				// Panel power and backlight are on the suspend rail
				PPR = 2

				// Self-refrech controller power rail
				CreateField(Local0, 5, 1, SRPR)
				// Remains powered while panel is powered
				SRPR = 0

				// FB power rail
				CreateField(Local0, 6, 2, FBPR)
				// FB is not on the suspend rail
				FBPR = 0

				// GPU power rail
				CreateField(Local0, 8, 2, GPR)
				// Combined power rail for all GPUs
				GPR = 0

				// GC6 ROM
				CreateField(Local0, 10, 1, GCR)
				// External SPI ROM
				GCR = 0

				// Panic trap handler
				CreateField(Local0, 11, 1, PTH)
				// No SMI handler
				PTH = 1

				// MS hybrid support
				CreateField(Local0, 13, 1, MHYB)
				// Supports deferred GC6 enter/exit in _PSx
				MHYB = 1

				// Root port control
				CreateField(Local0, 14, 1, RPC)
				// Supports fine grain root port control using PLON and PRPC
				RPC = 1

				// GC6 Version
				CreateField(Local0, 15, 2, GC6V)
				// GC6-R: standard sequence from chipset vendor
				GC6V = 2

				// Maximum revision supported
				CreateField(Local0, 20, 11, MXRV)
				MXRV = JT_REVISION_ID
			}
			Case (JT_FUNC_POWERCONTROL) {
				Printf("  JT_FUNC_POWERCONTROL: %o", SFST(Arg1))

				// GPU Power Control
				CreateField(Arg1, 0, 3, GPC)
				// Defer GC6 enter/exit until D3-Cold
				CreateField(Arg1, 14, 2, DFGC)
				// Deferred GC6 exit control
				CreateField(Arg1, 16, 3, GPCX)

				Switch (ToInteger(DFGC)) {
					// Activate power control immediately
					Case (0) {
						NVPC(GPC)
					}
					// Defer GPC and GPCX to be processed when setting power state
					// GC6 entry will take place in _PS3, exit in _PS0
					Case (1) {
						DPC = GPC
						DPCX = GPCX
					}
					// Clear any pending deferred requests
					Case (2) {
						DPC = 0
						DPCX = 0
					}
				}

				Local0 = Buffer (4) { 0, 0, 0, 0 }

				// GC Status
				CreateField(Local0, 0, 3, GCS)
				// GPU Power Status
				CreateField(Local0, 3, 1, GPS)

				//TODO: support D3-Cold state (2) ?
				If (GTXS (DGPU_RST_N)) {
					// GPU is in D0 state (powered on)
					GCS = 1
					GPS = 1
				} Else {
					// GPU is in GC6 state (powered off)
					GCS = 3
					GPS = 0
				}
			}
			Case (JT_FUNC_PLATPOLICY) {
				Printf("  JT_FUNC_PLATPOLICY")

				//TODO: audio enable/disable?

				Local0 = Buffer (4) { 0, 0, 0, 0 }
			}
			Default {
				Printf("  Unsupported JT_FUNC: %o, %o", SFST(Arg0), SFST(Arg1))
			}
		}

		Printf("  Return %o", SFST(Local0))
		Printf("} NVJT")
		Return (Local0)
	}

	#define NVOP_DSM_UUID "A486D8F8-0BDA-471B-A72B-6042A6B5BEE0"
	#define NVOP_REVISION_ID 0x0100

	#define NVOP_FUNC_SUPPORT 0
	#define NVOP_FUNC_OPTIMUSCAPS 0x1A
	#define NVOP_FUNC_OPTIMUSFLAGS 0x1B

	// PCIe configuration space owner current
	Name (PCOC, 0)
	// Optimus power control
	// 2 - platform should not power down GPU subsystem in _PS3
	// 3 - platform should power down GPU subsystem at the end of _PS3
	Name (OPPW, 2)

	Method (NVOP, 2, Serialized) {
		Printf("NVOP {")
		Local0 = NVIDIA_ERROR_UNSUPPORTED

		Switch (ToInteger(Arg0)) {
			Case (NVOP_FUNC_SUPPORT) {
				Printf("  NVOP_FUNC_SUPPORT")

				Local0 = Buffer (4) { 0, 0, 0, 0 }

				CreateField(Local0, NVOP_FUNC_SUPPORT, 1, FNSP)
				FNSP = 1

				CreateField(Local0, NVOP_FUNC_OPTIMUSCAPS, 1, FNCP)
				FNCP = 1
			}
			Case (NVOP_FUNC_OPTIMUSCAPS) {
				Printf("  NVOP_FUNC_OPTIMUSCAPS: %o", SFST(Arg1))

				// Flag update
				CreateField(Arg1, 0, 1, FLUP)

				// PCIe configuration space owner target
				CreateField(Arg1, 1, 1, PCOT)
				// PCIe configuration space owner write enable
				CreateField(Arg1, 2, 1, PCOW)
				// Update current PCIe configuration space owner if requested
				If (ToInteger(PCOW)) {
					PCOC = PCOT
				}

				// Optimus power control enable
				CreateField(Arg1, 24, 2, OPCE)
				// Update optimus power control if requested
				If (ToInteger(FLUP)) {
					OPPW = OPCE
				}

				Local0 = Buffer (4) { 0, 0, 0, 0 }

				// Optimus enabled
				CreateField(Local0, 0, 1, OPEN)
				// Optimus is enabled
				OPEN = 1

				// Current GPU control status
				CreateField(Local0, 3, 2, CGCS)
				If (GTXS (DGPU_RST_N)) {
					// GPU is on
					CGCS = 3
				} Else {
					// GPU is off
					CGCS = 0
				}

				// Shared hot plug capabilities
				CreateField(Local0, 6, 1, SHPC)
				// GPU hot plug is connected to PCH
				SHPC = 1

				// PCIe configuration space owner actual
				CreateField(Local0, 8, 1, PCOA)
				// Return current PCIe configuration space owner
				PCOA = PCOC

				// Optimus power capabilities
				CreateField(Local0, 24, 3, OPPC)
				// Platform has dynamic GPU power control
				OPPC = 1

				// Optimus HD audio capabilities
				CreateField(Local0, 27, 2, OPHA)
				// Platform uses GPU HD audio and supports dynamic power state reporting
				OPHA = 3
			}
			Default {
				Printf("  Unsupported NVOP_FUNC: %o, %o", SFST(Arg0), SFST(Arg1))
			}
		}

		Printf("  Return %o", SFST(Local0))
		Printf("} NVOP")
		Return (Local0)
	}

	Method (_DSM, 4) {
		Printf("NVIDIA _DSM {")
		Printf("  Arg0: %o", IDST(Arg0))
		Printf("  Arg1: %o", SFST(Arg1))
		Printf("  Arg2: %o", SFST(Arg2))
		Printf("  Arg3: %o", SFST(Arg3))
		//TODO: evaluate error return values
		Local0 = NVIDIA_ERROR_UNSPECIFIED

		If (Arg0 == ToUUID(NBCI_DSM_GUID)) {
			// Only support pecified NBCI revision
			If (ToInteger(Arg1) == NBCI_REVISION_ID) {
				Printf("  Return NBCI")
				Printf("} NVIDIA _DSM")
				Return (NBCI(Arg2, Arg3))
			} Else {
				Printf("  Unsupported NBCI_REVISION_ID: %o", SFST(Arg1))
			}
		} ElseIf (Arg0 == ToUUID(JT_DSM_GUID)) {
			// Only support specified NVIDIA JT revision and lower
			If (ToInteger(Arg1) <= JT_REVISION_ID) {
				Local0 = NVJT(Arg2, Arg3)
			} Else {
				Printf("  Unsupported JT_REVISION_ID: %o", SFST(Arg1))
			}
		} ElseIf (Arg0 == ToUUID(NVOP_DSM_UUID)) {
			// Only support specified NVIDIA OP revision
			If (ToInteger(Arg1) == NVOP_REVISION_ID) {
				Local0 = NVOP(Arg2, Arg3)
			} Else {
				Printf("  Unsupported NVOP_REVISION_ID: %o", SFST(Arg1))
			}
		} Else {
			Printf("  Unsupported DSM: %o, %o, %o, %o", IDST(Arg0), SFST(Arg1), SFST(Arg2), SFST(Arg3))
		}

		Printf("  Return %o", SFST(Local0))
		Printf("} NVIDIA _DSM")
		Return (Local0)
	}
// } NVIDIA DSMs

// RTD3 {
	// Memory mapped root port PCI express registers
	// Not sure what this stuff is, but it is used to get into GC6
        //TODO: use DGPU_DEVICE to generate address
	OperationRegion (RPCX, SystemMemory, CONFIG_MMCONF_BASE_ADDRESS + 0x8000, 0x1000)
	Field (RPCX, ByteAcc, NoLock, Preserve) {
		PVID,   16,
		PDID,   16,
		CMDR,   8,
		Offset (0x19),
		PRBN,   8,
		Offset (0x84),
		D0ST,   2,
		Offset (0xAA),
		CEDR,   1,
		Offset (0xAC),
			,   4,
		CMLW,   6,
		Offset (0xB0),
		ASPM,   2,
			,   2,
		P0LD,   1,
		RTLK,   1,
		Offset (0xC9),
			,   2,
		LREN,   1,
		Offset (0x11A),
			,   1,
		VCNP,   1,
		Offset (0x214),
		Offset (0x216),
		P0LS,   4,
		Offset (0x248),
			,   7,
		Q0L2,   1,
		Q0L0,   1,
		Offset (0x504),
		Offset (0x506),
		PCFG,   2,
		Offset (0x508),
		TREN,   1,
		Offset (0xC20),
			,   4,
		P0AP,   2,
		Offset (0xC38),
			,   3,
		P0RM,   1,
		Offset (0xC74),
		P0LT,   4,
		Offset (0xD0C),
			,   20,
		LREV,   1
	}

	// Enable link for RTD3
	Method (RTEN) {
		Printf("RTEN {")

		Printf("  Q0L0 = 1")
		Q0L0 = 1

		Printf("  Sleep 16")
		Sleep (16)

		Printf("  While Q0L0")
		Local0 = 0
		While (Q0L0) {
			If ((Local0 > 4)) {
				Printf("    While Q0L0 timeout")
				Break
			}

			Sleep (16)
			Local0++
		}

		Printf("  P0RM = 0")
		P0RM = 0

		Printf("  P0AP = 0")
		P0AP = 0

		Printf("} RTEN")
	}

	// Disable link for RTD3
	Method (RTDS) {
		Printf("RTDS {")

		Printf("  Q0L2 = 1")
		Q0L2 = 1

		Printf("  Sleep 16")
		Sleep (16)

		Printf("  While Q0L2")
		Local0 = Zero
		While (Q0L2) {
			If ((Local0 > 4)) {
				Printf("    While Q0L2 timeout")
				Break
			}

			Sleep (16)
			Local0++
		}

		Printf("  P0RM = 1")
		P0RM = 1

		Printf("  P0AP = 3")
		P0AP = 3

		Printf("} RTDS")
	}
// } RTD3

// GC6 {
	// Enter GC6
	Method (GC6I) {
		Printf("NVIDIA GC6I {")

		Printf("  LTRE = %o", SFST(LREN))
		LTRE = LREN

		//TODO: SIOT?

		RTDS()

		Printf("  Sleep 10")
		Sleep(10)

		Printf("  DGPU_RST_N low")
		CTXS(DGPU_RST_N)

		Printf("} NVIDIA GC6I")
	}

	// Exit GC6
	Method (GC6O) {
		Printf("NVIDIA GC6O {")

		//TODO: SIOT?

		Printf("  DGPU_RST_N high")
		STXS(DGPU_RST_N)

		Printf("  Sleep 10")
		Sleep (10)

		RTEN()

		Printf("  LREN = %o", SFST(LTRE))
		LREN = LTRE

		Printf("  CEDR = 1")
		CEDR = 1

		Printf("  CMDR |= 7")
		CMDR |= 7

		//TODO: SSID?

		Printf("} NVIDIA GC6O")
	}
// } GC6

#ifdef DGPU_GC6

	// GC6 3.0 implementation
	Name (LTRE, 0)

	Method (_ON) {
		Printf("NVIDIA _ON {")

		If (_STA != 0xF) {
			Printf("  If DGPU_PWR_EN low")
			If (! GTXS (DGPU_PWR_EN)) {
				Printf("    DGPU_PWR_EN high")
				STXS (DGPU_PWR_EN)

				Printf("    Sleep 16")
				Sleep (16)
			}

			GC6O()

			Printf("  _STA = 0xF")
			_STA = 0xF
		}

		Printf("} NVIDIA _ON")
	}

	Method (_OFF) {
		Printf("NVIDIA _OFF {")

		If (_STA != 0x5) {
			GC6I()

			Printf("  While DGPU_GC6 low")
			Local0 = Zero
			While (! GRXS(DGPU_GC6)) {
				If ((Local0 > 4)) {
					Printf("    While DGPU_GC6 low timeout")

					Printf("    DGPU_PWR_EN low")
					CTXS (DGPU_PWR_EN)
					Break
				}

				Sleep (16)
				Local0++
			}

			Printf("  _STA = 0x5")
			_STA = 0x5
		}

		Printf("} NVIDIA _OFF")
	}

#else // DGPU_GC6

	// GC6 2.1 implementation
	Name (LTRE, 0)

	// GPU power
	#define DLPW 7
	#define DLHR 2
	Method (GPPR, 1) {
		Printf("NVIDIA GPPR: %o {", Arg0)

		If (Arg0) {
			Printf("  DGPU_PWR_EN high")
			STXS (DGPU_PWR_EN)

			Printf("  Sleep %o", SFST(DLPW))
			Sleep (DLPW)

			Printf("  DGPU_RST_N high")
			STXS(DGPU_RST_N)

			Printf("  Sleep %o", SFST(DLHR))
			Sleep (DLHR)
		} Else {
			Printf("  DGPU_RST_N low")
			CTXS(DGPU_RST_N)

			Printf("  Sleep %o", SFST(DLHR))
			Sleep (DLHR)

			Printf("  DGPU_PWR_EN low")
			CTXS (DGPU_PWR_EN)
		}

		Printf("} NVIDIA GPPR: %o", Arg0)
	}

	// Memory mapped GPU PCI configuration space
    OperationRegion (PCS0, SystemMemory, 0xE0100000, 0x50)
    Field (PCS0, DWordAcc, Lock, Preserve) {
        Offset (0x40),
        SSID,   32 // Subsystem IDs
    }

	Method (_ON) {
		Printf("NVIDIA _ON {")

		/*
		If (_STA != 0xF) {
			GPPR(1)

			RTEN()

			//TODO: evaluate
			Printf("  While P0LS < 7")
			While (P0LS < 7) {
				Local0 = 0x20
				While (Local0) {
					If (P0LS < 7) {
						Stall (100)
						Local0--
					} Else {
						Break
					}
				}

				If (Local0 == 0) {
					RTLK = 1
					Stall (100)
				}
			}

			Printf("  LREN = %o", SFST(LTRE))
			LREN = LTRE

			Printf("  CEDR = 1")
			CEDR = 1

			Printf("  CMDR |= 7")
			CMDR |= 7

			//TODO: what is this?
			Printf("  D0ST = 0")
			D0ST = 0

			Local0 = (CONFIG_SUBSYSTEM_DEVICE_ID << 16) | CONFIG_SUBSYSTEM_VENDOR_ID
			Printf("  SSID = %o", SFST(Local0))
			SSID = Local0

			Printf("  _STA = 0xF")
			_STA = 0xF
		}
		*/

		Printf("} NVIDIA _ON")
	}

	Method (_OFF) {
		Printf("NVIDIA _OFF {")

		/*
		If (_STA != 0x5) {
			Printf("  LTRE = %o", SFST(LREN))
			LTRE = LREN

			RTDS()

			GPPR(0)

			Printf("  _STA = 0x5")
			_STA = 0x5
		}
		*/

		Printf("} NVIDIA _OFF")
	}
#endif // DGPU_GC6
}
