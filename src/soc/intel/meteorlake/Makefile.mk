## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_INTEL_METEORLAKE),y)

subdirs-y += romstage
subdirs-y += ../../../cpu/intel/microcode
subdirs-y += ../../../cpu/intel/turbo

# all (bootblock, verstage, romstage, postcar, ramstage)
all-y += gspi.c
all-y += i2c.c
all-y += pmutil.c
all-y += spi.c
all-y += uart.c
all-y += gpio.c

bootblock-y += bootblock/bootblock.c
bootblock-y += bootblock/ioe_die.c
bootblock-y += bootblock/report_platform.c
bootblock-y += bootblock/soc_die.c
bootblock-y += espi.c
bootblock-y += soc_info.c

romstage-$(CONFIG_SOC_INTEL_CSE_PRE_CPU_RESET_TELEMETRY) += cse_telemetry.c
romstage-y += espi.c
romstage-y += meminit.c
romstage-y += pcie_rp.c
romstage-y += reset.c
romstage-y += soc_info.c

ramstage-y += acpi.c
ramstage-y += chip.c
ramstage-y += cpu.c
ramstage-$(CONFIG_SOC_INTEL_CRASHLOG) += crashlog.c
ramstage-y += ioe_pmc.c
ramstage-y += elog.c
ramstage-y += espi.c
ramstage-y += finalize.c
ramstage-y += fsp_params.c
ramstage-y += graphics.c
ramstage-y += lockdown.c
ramstage-y += p2sb.c
ramstage-y += pcie_rp.c
ramstage-y += pmc.c
ramstage-y += reset.c
ramstage-y += retimer.c
ramstage-y += soundwire.c
ramstage-y += systemagent.c
ramstage-y += tcss.c
ramstage-y += xhci.c
ramstage-y += soc_info.c

smm-y += elog.c
smm-y += gpio.c
smm-y += p2sb.c
smm-y += pmutil.c
smm-y += smihandler.c
smm-y += soc_info.c
smm-y += uart.c
smm-y += xhci.c
CPPFLAGS_common += -I$(src)/soc/intel/meteorlake
CPPFLAGS_common += -I$(src)/soc/intel/meteorlake/include

ifeq ($(CONFIG_SOC_INTEL_METEORLAKE_U_H),y)
cpu_microcode_bins += 3rdparty/intel-microcode/intel-ucode/06-aa-04
endif

endif
