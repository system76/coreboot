/* SPDX-License-Identifier: GPL-2.0-only */

// Notebook Common Interface

#define NBCI_FUNC_SUPPORT		0
#define NBCI_FUNC_PLATCAPS		1
#define NBCI_FUNC_PLATPOLICY		4
#define NBCI_FUNC_DISPLAYSTATUS		5
#define NBCI_FUNC_GETOBJBYTYPE		16
#define NBCI_FUNC_GETALLOBJS		17
#define NBCI_FUNC_GETEVENTLIST		18
#define NBCI_FUNC_CALLBACKS		29
#define NBCI_FUNC_GETBACKLIGHT		20
#define NBCI_FUNC_GETLICENSE		22
#define NBCI_FUNC_GETEFITABLE		23

Scope (\_SB.PCI0.PEG0.DGPU)
{
	Method (NBCI, 2, NotSerialized)
	{
		Printf("NBCI {")
		Local0 = NVIDIA_ERROR_UNSUPPORTED

		Switch (ToInteger(Arg0)) {
			// Bit list of supported functions
			Case (NBCI_FUNC_SUPPORT) {
				// Supported functions: 0
				Local0 = Buffer() {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
			}

			// Query Plaform Capabilities
			Case (NBCI_FUNC_PLATCAPS) {
				Printf("  NBCI_FUNC_PLATCAPS: Unimplemented!")
			}

			// Query Platform Policies
			Case (NBCI_FUNC_PLATPOLICY) {
				Printf("  NBCI_FUNC_PLATPOLICY: Unimplemented!")
			}

			// Query Display status
			Case (NBCI_FUNC_DISPLAYSTATUS) {
				Printf("  NBCI_FUNC_DISPLAYSTATUS: Unimplemented!")
			}

			// Fetch and specific Object by Type
			Case (NBCI_FUNC_GETOBJBYTYPE) {
				Printf("  NBCI_FUNC_GETOBJBYTYPE: Unimplemented!")
			}

			// Fetch all Objects
			Case (NBCI_FUNC_GETALLOBJS) {
				Printf("  NBCI_FUNC_GETALLOBJS: Unimplemented!")
			}

			// Get list of Notify events and their meaning
			Case (NBCI_FUNC_GETEVENTLIST) {
				Printf("  NBCI_FUNC_GETEVENTLIST: Unimplemented!")
			}

			// Get list of system-required callbacks
			Case (NBCI_FUNC_CALLBACKS) {
				Printf("  NBCI_FUNC_CALLBACKS: Unimplemented!")
			}

			// Get the Backlight setup settings
			Case (NBCI_FUNC_GETBACKLIGHT) {
				Printf("  NBCI_FUNC_GETBACKLIGHT: Unimplemented!")
			}

			// Get Software License Number
			Case (NBCI_FUNC_GETLICENSE) {
				Printf("  NBCI_FUNC_GETLICENSE: Unimplemented!")
			}

			// Get EFI System Table
			Case (NBCI_FUNC_GETEFITABLE) {
				Printf("  NBCI_FUNC_GETEFITABLE: Unimplemented!")
			}

			Default {
				Printf("  Unsupported NBCI_FUNC: %o", ToInteger(Arg0))
				Local0 = NVIDIA_ERROR_UNSUPPORTED
			}
		}

		Printf("} NBCI")
		Return(Local0)
	}
}
