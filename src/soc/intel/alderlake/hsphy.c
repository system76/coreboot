/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__

#include <assert.h>
#include <stdlib.h>
#include <console/console.h>
#include <device/device.h>
#include <device/mmio.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <intelblocks/cse.h>
#include <intelblocks/systemagent.h>
#include <soc/hsphy.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <vb2_api.h>
#include <lib.h>
#include <delay.h>

#define HASHALG_SHA1		0x00000001
#define HASHALG_SHA256		0x00000002
#define HASHALG_SHA384		0x00000003
#define HASHALG_SHA512		0x00000004

#define MAX_HASH_SIZE		64 // 64 is max size for SHA512
#define GET_IP_FIRMWARE_CMD	0x21
#define HSPHY_PAYLOAD_SIZE		32*KiB

#define CPU_PID_PCIE_BUTTRESSx16_0	0x54
#define CPU_PID_PCIE_PHYX16_BROADCAST	0x55
#define CPU_PID_PCIE_PHYx16_0		0x56
#define CPU_PID_PCIE_PHYx16_1		0x57

#define HSPHY_LOADER_BASE				0
#define HSPHY_VERIFIER_BASE				0x3000
#define   HSPHY_MEM_IP_CAP_0				(HSPHY_VERIFIER_BASE + 0x18)
#define   HSPHY_MEM_IP_CAP_0_BIOS_LOADER		12
#define   HSPHY_MEM_IP_CAP_0_MASK			0xF
#define   HSPHY_MEM_VERIFIER_ERROR_STATUS 		(HSPHY_VERIFIER_BASE + 0x30)
#define   HSPHY_MEM_VERIFIER_ERROR_STATUS_ERROR_CODE	4

#define HSPHY_RECIPE_VERSION				0x314
#define   HSPHY_RECIPE_VERSION_MINOR_MASK		0xF
#define   HSPHY_RECIPE_VERSION_MINOR_OFFSET		0
#define   HSPHY_RECIPE_VERSION_MAJOR_MASK		0xF0
#define   HSPHY_RECIPE_VERSION_MAJOR_OFFSET		4

#define HSPHY_FW_VERSION				0x1C
#define   HSPHY_FW_VERSION_PROD_MAJOR_MASK		0xF0000000
#define   HSPHY_FW_VERSION_PROD_MAJOR_OFFSET		28
#define   HSPHY_FW_VERSION_PROD_MINOR_MASK		0xFF00000
#define   HSPHY_FW_VERSION_PROD_MINOR_OFFSET		20
#define   HSPHY_FW_VERSION_HOTFIX_MASK			0xF0000
#define   HSPHY_FW_VERSION_HOTFIX_OFFSET		16
#define   HSPHY_FW_VERSION_BUILD_MASK			0xF000
#define   HSPHY_FW_VERSION_BUILD_OFFSET			12
#define   HSPHY_FW_VERSION_EV_BITPROG_MAJOR_MASK	0xF00
#define   HSPHY_FW_VERSION_EV_BITPROG_MAJOR_OFFSET	8
#define   HSPHY_FW_VERSION_EV_BITPROG_MINOR_MASK	0xFF
#define   HSPHY_FW_VERSION_EV_BITPROG_MINOR_OFFSET	0

struct ip_push_model {
	uint16_t count;
	uint16_t address;
	uint32_t data[0];
}__packed;

static int heci_get_hsphy_payload(void *buf, uint32_t *buf_size, uint8_t *hash_buf,
				  uint8_t *hash_alg, uint32_t *status)
{
	size_t reply_size;

	struct heci_ip_load_request {
		struct mkhi_hdr hdr;
		uint32_t version;
		uint32_t operation;
		uint32_t dram_base_low;
		uint32_t dram_base_high;
		uint32_t memory_size;
		uint32_t reserved;
	} __packed msg = {
		.hdr = {
			.group_id = MKHI_GROUP_ID_BUP_COMMON,
			.command = GET_IP_FIRMWARE_CMD,
		},
		.version = 1,
		.operation = 1,
		.dram_base_low = (uintptr_t)buf,
		.dram_base_high = 0,
		.memory_size = *buf_size,
		.reserved = 0,
	};

	struct heci_ip_load_response {
		struct mkhi_hdr hdr;
		uint32_t payload_size;
		uint32_t reserved[2];
		uint32_t status;
		uint8_t hash_type;
		uint8_t hash[MAX_HASH_SIZE];
	} __packed reply;

	if (!buf || !buf_size || !hash_buf || !hash_alg) {
		printk(BIOS_ERR, "%s: Invalid parameters\n", __func__);
		return -1;
	}

	reply_size = sizeof(reply);
	memset(&reply, 0, reply_size);

	printk(BIOS_DEBUG, "HECI: Sending Get IP firmware command\n");

	if (heci_send_receive(&msg, sizeof(msg), &reply, &reply_size, HECI_MKHI_ADDR)) {
		printk(BIOS_ERR, "HECI: Get IP firmware failed\n");
		return -1;
	}

	if (reply.hdr.result) {
		printk(BIOS_ERR, "HECI: Get IP firmware response invalid\n");
		*status = reply.status;
		hexdump(&reply, sizeof(reply));
		return -1;
	}

	*buf_size = reply.payload_size;
	*hash_alg = reply.hash_type;
	*status = reply.status;
	memcpy(hash_buf, reply.hash, MAX_HASH_SIZE);

	printk(BIOS_DEBUG, "HECI: Get IP firmware success. Response:\n");
	printk(BIOS_DEBUG, "  Payload size = 0x%x\n", *buf_size);
	printk(BIOS_DEBUG, "  Hash type used for signing payload = 0x%x\n", *hash_alg);

	return 0;
}

static int verify_hsphy_hash(void *buf, uint32_t buf_size, uint8_t *hash_buf, uint8_t hash_alg)
{
	enum vb2_hash_algorithm alg;
	uint32_t hash_size;
	uint8_t hash_calc[MAX_HASH_SIZE];

	switch (hash_alg) {
	case HASHALG_SHA256:
		alg = VB2_HASH_SHA256;
		hash_size = VB2_SHA256_DIGEST_SIZE;
		break;
	case HASHALG_SHA384:
		alg = VB2_HASH_SHA384;
		hash_size = VB2_SHA384_DIGEST_SIZE;
		break;
	case HASHALG_SHA512:
		alg = VB2_HASH_SHA512;
		hash_size = VB2_SHA512_DIGEST_SIZE;
		break;
	case HASHALG_SHA1:
	default:
		printk (BIOS_ERR, "Hash alg %d not supported, try SHA384\n", hash_alg);
		alg = VB2_HASH_SHA384;
		hash_size = VB2_SHA384_DIGEST_SIZE;
		break;
	}

	if (vb2_digest_buffer(buf, buf_size, alg, hash_calc, hash_size)) {
		printk(BIOS_ERR, "HSPHY SHA calculation failed\n");
		return -1;
	}

	if (memcmp(hash_buf, hash_calc, hash_size)) {
		printk(BIOS_ERR, "HSPHY SHA hashes do not match\n");
		hexdump(hash_buf, hash_size);
		hexdump(hash_calc, hash_size);
		return -1;
	}

	return 0;
}

static void upload_hsphy_to_cpu_pcie(void *buf, uint32_t buf_size)
{
	uint32_t i = 0, j;
	struct ip_push_model *push_model = (struct ip_push_model *)buf;

	while (i < buf_size) {
		i += sizeof (*push_model);

		if ((push_model->address == 0) && (push_model->count == 0)) {
			break; // End of file
		}
		for (j = 0; j < push_model->count; j++) {
			REGBAR32(CPU_PID_PCIE_PHYX16_BROADCAST,
				 push_model->address) = push_model->data[j];
			i += sizeof (uint32_t);
		}

		push_model = (struct ip_push_model *)(buf + i);
	}
}

static bool is_bios_hsphy_loader(void)
{
	uint64_t cap = REGBAR64(CPU_PID_PCIE_BUTTRESSx16_0, HSPHY_MEM_IP_CAP_0);

	if ((cap >> HSPHY_MEM_IP_CAP_0_BIOS_LOADER) & HSPHY_MEM_IP_CAP_0_MASK) {
		printk(BIOS_DEBUG, "HSPHY: BIOS is the loader of HSPHY firmware\n");
		return true;
	}

	printk(BIOS_DEBUG, "HSPHY: BIOS is not the loader of HSPHY firmware\n");
	return false;
}

static void set_hsphy_verifier_error_code(uint8_t image_number, uint32_t error_code)
{
	uint32_t error = REGBAR32(CPU_PID_PCIE_BUTTRESSx16_0, HSPHY_MEM_VERIFIER_ERROR_STATUS);

	error |= (error_code << HSPHY_MEM_VERIFIER_ERROR_STATUS_ERROR_CODE);
	error |= (1 << image_number);

	REGBAR32(CPU_PID_PCIE_BUTTRESSx16_0, HSPHY_MEM_VERIFIER_ERROR_STATUS) = error;
}

void load_and_init_hsphy(void)
{
	void *hsphy_buf;
	uint8_t hsphy_hash[MAX_HASH_SIZE] = { 0 };
	uint8_t hash_type;
	uint32_t buf_size = HSPHY_PAYLOAD_SIZE;
	pci_devfn_t dev = PCH_DEV_CSE;
	uint16_t pci_cmd_bme_mem = PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY;
	uint32_t status;

	if (!is_devfn_enabled(SA_DEVFN_CPU_PCIE1_0) &&
	    !is_devfn_enabled(SA_DEVFN_CPU_PCIE1_1)) {
		printk(BIOS_DEBUG, "All HSPHY ports disabled, skipping HSPHY loading\n");
	}

	if (!is_bios_hsphy_loader())
		return;

	/* Align the buffer to page size, otherwise the HECI command will fail */
	hsphy_buf = memalign(4*KiB, HSPHY_PAYLOAD_SIZE);

	if (!hsphy_buf) {
		printk(BIOS_ERR, "Could not allocate memory for HSPHY blob\n");
		return;
	}

	memset(hsphy_buf, 0, HSPHY_PAYLOAD_SIZE);

	if (!is_cse_enabled()) {
		printk(BIOS_ERR, "%s: CSME not enabled or not visible\n", __func__);
		return;
	}

	/* Ensure BAR, BME and memory space are enabled */
	if ((pci_read_config16(dev, PCI_COMMAND) & pci_cmd_bme_mem) != pci_cmd_bme_mem)
		pci_or_config16(dev, PCI_COMMAND, pci_cmd_bme_mem);


	if (pci_read_config32(dev, PCI_BASE_ADDRESS_0) == 0) {
		pci_and_config16(dev, PCI_COMMAND, ~pci_cmd_bme_mem);
		pci_write_config32(dev, PCI_BASE_ADDRESS_0, HECI1_BASE_ADDRESS);
		pci_or_config16(dev, PCI_COMMAND, pci_cmd_bme_mem);
	}

	if (heci_get_hsphy_payload(hsphy_buf, &buf_size, hsphy_hash, &hash_type, &status)) {
		set_hsphy_verifier_error_code(1, status);
		return;
	}

	if (verify_hsphy_hash(hsphy_buf, buf_size, hsphy_hash, hash_type))
		return;
	
	upload_hsphy_to_cpu_pcie(hsphy_buf, buf_size);

	free(hsphy_buf);
}

static bool is_hsphy_fw_download_complete(uint32_t cpu_pcie_dev)
{
	uint16_t pid;

	switch (cpu_pcie_dev) {
	case SA_DEVFN_CPU_PCIE1_0:
		pid = CPU_PID_PCIE_PHYx16_0;
		break;
	case SA_DEVFN_CPU_PCIE1_1:
		pid = CPU_PID_PCIE_PHYx16_1;
		break;
	default:
		printk(BIOS_ERR, "%s: incorrect CPU PCIe devfn\n", __func__);
		return false;
	}
	return (REGBAR32(pid, HSPHY_RECIPE_VERSION) == 0) ? false : true;
}

void print_hsphy_version(uint32_t cpu_pcie_dev)
{

	uint32_t hsphy_ver, hsphy_recipe;

	if (!is_hsphy_fw_download_complete(cpu_pcie_dev)) {
		printk(BIOS_ERR, "%s: HSPHY firmware download not complete\n", __func__);
		return;
	}

	switch (cpu_pcie_dev) {
	case SA_DEVFN_CPU_PCIE1_0:
		printk(BIOS_DEBUG, "Printing HSPHY version for CPU PCIe 0:1.0\n");
		hsphy_ver = REGBAR32(CPU_PID_PCIE_PHYx16_0, HSPHY_FW_VERSION);;
		hsphy_recipe = REGBAR32(CPU_PID_PCIE_PHYx16_0, HSPHY_RECIPE_VERSION);
		break;
	case SA_DEVFN_CPU_PCIE1_1:
		printk(BIOS_DEBUG, "Printing HSPHY version for CPU PCIe 0:1.1\n");
		hsphy_ver = REGBAR32(CPU_PID_PCIE_PHYx16_1, HSPHY_FW_VERSION);;
		hsphy_recipe = REGBAR32(CPU_PID_PCIE_PHYx16_1, HSPHY_RECIPE_VERSION);
		break;
	default:
		printk(BIOS_ERR, "%s: incorrect CPU PCIe devfn\n", __func__);
		return;
	}

	printk(BIOS_DEBUG, "HSPHY: Recipe version: %d.%d\n",
		(hsphy_recipe & HSPHY_RECIPE_VERSION_MAJOR_MASK) >>
				HSPHY_RECIPE_VERSION_MAJOR_OFFSET,
		(hsphy_recipe & HSPHY_RECIPE_VERSION_MINOR_MASK) >>
				HSPHY_RECIPE_VERSION_MINOR_OFFSET);

	printk(BIOS_DEBUG, "HSPHY: Firmware version: %d.%d.%d.%d.%d.%d\n",
		(hsphy_ver & HSPHY_FW_VERSION_PROD_MAJOR_MASK) >>
			     HSPHY_FW_VERSION_PROD_MAJOR_OFFSET,
		(hsphy_ver & HSPHY_FW_VERSION_PROD_MINOR_MASK) >>
			     HSPHY_FW_VERSION_PROD_MAJOR_OFFSET,
		(hsphy_ver & HSPHY_FW_VERSION_HOTFIX_MASK) >>
			     HSPHY_FW_VERSION_HOTFIX_OFFSET,
		(hsphy_ver & HSPHY_FW_VERSION_BUILD_MASK) >>
			     HSPHY_FW_VERSION_BUILD_OFFSET,
		(hsphy_ver & HSPHY_FW_VERSION_EV_BITPROG_MAJOR_MASK) >>
			     HSPHY_FW_VERSION_EV_BITPROG_MAJOR_OFFSET,
		(hsphy_ver & HSPHY_FW_VERSION_EV_BITPROG_MINOR_MASK) >>
			     HSPHY_FW_VERSION_EV_BITPROG_MINOR_OFFSET);
}