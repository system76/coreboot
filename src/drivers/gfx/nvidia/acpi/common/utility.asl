/* SPDX-License-Identifier: GPL-2.0-only */

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

// Convert from 4-byte buffer to 32-bit integer
Method (BTOI, 1) {
	Return(
		DerefOf(Arg0[0]) |
		(DerefOf(Arg0[1]) << 8) |
		(DerefOf(Arg0[2]) << 16) |
		(DerefOf(Arg0[3]) << 24)
	)
}

// Convert from 32-bit integer to 4-byte buffer
Method (ITOB, 1) {
	Local0 = Buffer(4) { 0, 0, 0, 0 }
	Local0[0] = Arg0 & 0xFF
	Local0[1] = (Arg0 >> 8) & 0xFF
	Local0[2] = (Arg0 >> 16) & 0xFF
	Local0[3] = (Arg0 >> 24) & 0xFF
	Return (Local0)
}
