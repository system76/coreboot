Device (\_SB.PCI0.PEG0) {
	Name (_ADR, 0x00010000)

	Device (PEGP) {
		Name (_ADR, Zero)

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

        Method (_DSM, 4, Serialized) {
            Debug = "NVIDIA _DSM"
            Printf("  Arg0: %o", IDST(Arg0))
            Printf("  Arg1: %o", SFST(Arg1))
            Printf("  Arg2: %o", SFST(Arg2))
            Printf("  Arg3: %o", SFST(Arg3))

            If (Arg0 == ToUUID ("d4a50b75-65c7-46f7-bfb7-41514cea0244")) {
                If (Arg1 != 0x0102) {
                    Printf("  Invalid Arg1, return 0x80000002")
                    Return (0x80000002)
                }

                If (Arg2 == 0) {
                    Printf("  Arg2 == 0x00, return some buffer")
                    Return (Buffer (4) {
                         0x01, 0x00, 0x11, 0x00
                    })
                }

                If (Arg2 == 0x10) {
                    CreateWordField (Arg3, 0x02, BFF0)
                    Printf("  Arg2 = 0x10, return buffer %o", ToHexString(BFF0))
                    If (BFF0 == 0x564B) { // 'VK'
                        Printf("  Return GVK")
                        Return (Buffer (0xD5) {
                            0xA6,
                            0x92,
                            0x17,
                            0xE2,
                            0x85,
                            0x2A,
                            0xED,
                            0x48,
                            0x4B,
                            0x56,
                            0xD5,
                            0x00,
                            0x00,
                            0x00,
                            0x01,
                            0x00,
                            0x37,
                            0x35,
                            0x31,
                            0x31,
                            0x31,
                            0x35,
                            0x38,
                            0x37,
                            0x39,
                            0x38,
                            0x34,
                            0x39,
                            0x47,
                            0x65,
                            0x6E,
                            0x75,
                            0x69,
                            0x6E,
                            0x65,
                            0x20,
                            0x4E,
                            0x56,
                            0x49,
                            0x44,
                            0x49,
                            0x41,
                            0x20,
                            0x43,
                            0x65,
                            0x72,
                            0x74,
                            0x69,
                            0x66,
                            0x69,
                            0x65,
                            0x64,
                            0x20,
                            0x47,
                            0x53,
                            0x79,
                            0x6E,
                            0x63,
                            0x20,
                            0x52,
                            0x65,
                            0x61,
                            0x64,
                            0x79,
                            0x20,
                            0x50,
                            0x6C,
                            0x61,
                            0x74,
                            0x66,
                            0x6F,
                            0x72,
                            0x6D,
                            0x20,
                            0x66,
                            0x6F,
                            0x72,
                            0x20,
                            0x43,
                            0x4C,
                            0x45,
                            0x56,
                            0x4F,
                            0x20,
                            0x32,
                            0x30,
                            0x30,
                            0x35,
                            0x39,
                            0x30,
                            0x38,
                            0x38,
                            0x30,
                            0x20,
                            0x20,
                            0x20,
                            0x20,
                            0x20,
                            0x20,
                            0x2D,
                            0x20,
                            0x31,
                            0x39,
                            0x27,
                            0x3B,
                            0x36,
                            0x28,
                            0x53,
                            0x5E,
                            0x26,
                            0x2B,
                            0x35,
                            0x39,
                            0x55,
                            0x56,
                            0x32,
                            0x30,
                            0x58,
                            0x44,
                            0x26,
                            0x32,
                            0x44,
                            0x5C,
                            0x23,
                            0x2E,
                            0x31,
                            0x39,
                            0x32,
                            0x49,
                            0x3E,
                            0x25,
                            0x3B,
                            0x41,
                            0x57,
                            0x50,
                            0x5A,
                            0x56,
                            0x2C,
                            0x3E,
                            0x5F,
                            0x38,
                            0x20,
                            0x2D,
                            0x20,
                            0x43,
                            0x6F,
                            0x70,
                            0x79,
                            0x72,
                            0x69,
                            0x67,
                            0x68,
                            0x74,
                            0x20,
                            0x32,
                            0x30,
                            0x31,
                            0x34,
                            0x20,
                            0x4E,
                            0x56,
                            0x49,
                            0x44,
                            0x49,
                            0x41,
                            0x20,
                            0x43,
                            0x6F,
                            0x72,
                            0x70,
                            0x6F,
                            0x72,
                            0x61,
                            0x74,
                            0x69,
                            0x6F,
                            0x6E,
                            0x20,
                            0x41,
                            0x6C,
                            0x6C,
                            0x20,
                            0x52,
                            0x69,
                            0x67,
                            0x68,
                            0x74,
                            0x73,
                            0x20,
                            0x52,
                            0x65,
                            0x73,
                            0x65,
                            0x72,
                            0x76,
                            0x65,
                            0x64,
                            0x2D,
                            0x35,
                            0x38,
                            0x39,
                            0x36,
                            0x38,
                            0x34,
                            0x30,
                            0x32,
                            0x39,
                            0x33,
                            0x38,
                            0x35,
                            0x28,
                            0x52,
                            0x29,
                        })
                    }

                    If (BFF0 == 0x4452) { // 'DR'
                        Printf("  Return GDK")
                        Return (Buffer (0xAA) {
                            0xB4,
                            0x6B,
                            0x02,
                            0x49,
                            0x41,
                            0x16,
                            0x1C,
                            0xD2,
                            0x52,
                            0x44,
                            0xAA,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0x01,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0xDE,
                            0x10,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0x09,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0x34,
                            0x00,
                            0x00,
                            0x00,
                            0x01,
                            0x00,
                            0x47,
                            0x00,
                            0x00,
                            0x00,
                            0x02,
                            0x00,
                            0x45,
                            0x00,
                            0x00,
                            0x00,
                            0x03,
                            0x00,
                            0x5A,
                            0x00,
                            0x00,
                            0x00,
                            0x04,
                            0x00,
                            0x58,
                            0x00,
                            0x00,
                            0x00,
                            0x05,
                            0x00,
                            0x56,
                            0x00,
                            0x00,
                            0x00,
                            0x06,
                            0x00,
                            0x54,
                            0x00,
                            0x00,
                            0x00,
                            0x07,
                            0x00,
                            0x52,
                            0x00,
                            0x00,
                            0x00,
                            0x08,
                            0x00,
                            0x50,
                            0x00,
                            0x00,
                            0x00,
                            0x01,
                            0x00,
                            0x00,
                            0x00,
                            0xD9,
                            0x1C,
                            0x04,
                            0x00,
                            0x00,
                            0x00,
                            0x02,
                            0x00,
                            0x00,
                            0x00,
                            0x41,
                            0x5D,
                            0xC9,
                            0x00,
                            0x01,
                            0x24,
                            0x2E,
                            0x00,
                            0x02,
                            0x00,
                            0xFF,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0x01,
                            0x00,
                            0x00,
                            0x00,
                            0xD9,
                            0x1C,
                            0x04,
                            0x00,
                            0x00,
                            0x00,
                            0x03,
                            0x00,
                            0x00,
                            0x00,
                            0xE0,
                            0x7C,
                            0x97,
                            0x01,
                            0xC1,
                            0x3D,
                            0x9C,
                            0x1B,
                            0x41,
                            0x60,
                            0x68,
                            0x9E,
                            0x35,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                        })
                    }
                }

                If (Arg2 == 0x14) {
                    Printf("  Arg2 == 0x14, return backlight package")
					Return (Package (9) {
						0x8000A450,
						0x0200,
						Zero,
						Zero,
						One,
						One,
						200,
						32,
						1000
					})
                }

                Printf("  Unknown Arg2, return 0x80000002")
                Return (0x80000002)
            }

            Printf("  Unknown Arg0, return 0x80000001")
            Return (0x80000001)
        }

        Method (_DOD, 0, NotSerialized)  // _DOD: Display Output Devices
        {
            Return (Package (3) {
                0x80008320,
                0x80006330,
                0x8000A450
            })
		}

        Device (HDM0) {
            Name (_ADR, 0x80008320)
        }

        Device (DSP0) {
            Name (_ADR, 0x80006330)
        }

        Device (DSP1) {
            Name (_ADR, 0x8000A450)
        }
	}
}
