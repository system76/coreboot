## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_INTEL_NOVALAKE_BASE),y)

subdirs-y += ../../../cpu/intel/microcode
subdirs-y += ../../../cpu/intel/turbo
subdirs-y += romstage

bootblock-y += bootblock/bootblock.c
bootblock-y += bootblock/pcd.c
bootblock-y += bootblock/report_platform.c

romstage-$(CONFIG_SOC_INTEL_CSE_PRE_CPU_RESET_TELEMETRY) += cse_telemetry.c
# espi.c is provided by SOC_INTEL_COMMON_FEATURE_ESPI
romstage-y += meminit.c
romstage-y += pcie_rp.c
romstage-y += reset.c

CPPFLAGS_common += -I$(src)/soc/intel/novalake
CPPFLAGS_common += -I$(src)/soc/intel/novalake/include

endif
