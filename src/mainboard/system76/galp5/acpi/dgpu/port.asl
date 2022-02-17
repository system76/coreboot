OperationRegion (L23C, PCI_Config, 0x00, 0xFF)
Field (L23C, DwordAcc, NoLock, Preserve) {
	Offset (0x52),
	, 13,
	LASX, 1,
	Offset (0xE0),
	, 7,
	NCB7, 1,
	Offset (0xE2),
	, 2,
	L23E, 1,
	L23R, 1,
	/*TODO
	Offset (0x420),
	, 30,
	DPGE, 1
	*/
}

// Enter L23
Method(DL23, 0, Serialized) {
	Printf("      GPU PORT DL23 START")
	L23E = 1
	Local0 = 8
	While ((Local0 > 0)) {
		If ((L23E == 0)) {
			Break
		}
		Sleep (16)
		Local0--
	}
	NCB7 = 1
	Printf("      GPU PORT DL23 FINISH")
}

// Exit L23
Method(L23D, 0, Serialized) {
	Printf("      GPU PORT L23D START")
	If ((NCB7 == 1)) {
		//TODO: DPGE = 0
		L23R = 1
		Local0 = 20
		While ((Local0 > 0)) {
			If ((L23R == 0)) {
				Break
			}
			Sleep (16)
			Local0--
		}

		NCB7 = 0
		//TODO: DPGE = 1
		Local0 = 8
		While ((Local0 > 0)) {
			If ((LASX == 1)) {
				Break
			}
			Sleep (16)
			Local0--
		}
	}
	Printf("      GPU PORT L23D FINISH")
}

Method (_STA, 0, Serialized) {
	//TODO
	Return (0xF)
}

// Main power resource
PowerResource (PWRR, 0, 0) {
	Name (_STA, 1)

	Method (_ON, 0, Serialized) {
		Printf("GPU PORT PWRR._ON")
		_STA = 1
	}

	Method (_OFF, 0, Serialized) {
		Printf("GPU PORT PWRR._OFF")
		_STA = 0
	}
}

// Power resources for entering D0
Name (_PR0, Package () { PWRR })

// Power resources for entering D3
Name (_PR3, Package () { PWRR })

// Current power state
Name (_PSC, 0)

// Place device in D0
Method (_PS0, 0, Serialized) {
	Printf("GPU PORT _PS0")
	_PSC = 0
}

// Place device in D3
Method (_PS3, 0, Serialized) {
	Printf("GPU PORT _PS3")
	_PSC = 3
}
