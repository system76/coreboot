/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/helpers.h>
#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <intelblocks/cse.h>
#include <intelblocks/p2sb.h>
#include <intelblocks/pcr.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>

#define CSME0_FBE	0xf
#define CSME0_BAR	0x0
#define CSME0_FID	0xb0

/* Disable HECI using Sideband interface communication */
void heci_disable(void)
{
	struct pcr_sbi_msg msg = {
		.pid = PID_CSME0,
		.offset = 0,
		.opcode = PCR_WRITE,
		.is_posted = false,
		.fast_byte_enable = CSME0_FBE,
		.bar = CSME0_BAR,
		.fid = CSME0_FID
	};
	/* Bit 0: Set to make HECI#1 Function disable */
	uint32_t data32 = 1;
	uint8_t response;
	int status;

	/* unhide p2sb device */
	p2sb_unhide();

	/* Send SBI command to make HECI#1 function disable */
	status = pcr_execute_sideband_msg(PCH_DEV_P2SB, &msg, &data32, &response);
	if (status || response)
		printk(BIOS_ERR, "Fail to make CSME function disable\n");

	/* Ensure to Lock SBI interface after this command */
	p2sb_disable_sideband_access();

	/* hide p2sb device */
	p2sb_hide();
}
