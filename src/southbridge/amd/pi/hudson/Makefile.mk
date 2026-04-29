## SPDX-License-Identifier: BSD-3-Clause

# Copyright (c) 2012,        Advanced Micro Devices, Inc.
#               2013 - 2014, Sage Electronic Engineering, LLC

bootblock-y += bootblock.c
bootblock-y += early_setup.c
bootblock-$(CONFIG_USBDEBUG) += enable_usbdebug.c

romstage-y += early_setup.c
romstage-y += enable_usbdebug.c
romstage-$(CONFIG_HUDSON_IMC_FWM) += imc.c
romstage-y += smbus.c
romstage-y += smbus_spd.c

verstage-y += early_setup.c

ramstage-y += enable_usbdebug.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += fadt.c
ramstage-y += hda.c
ramstage-y += hudson.c
ramstage-y += ide.c
ramstage-$(CONFIG_HUDSON_IMC_FWM) += imc.c
ramstage-y += lpc.c
ramstage-y += pci.c
ramstage-y += pcie.c
ramstage-y += sata.c
ramstage-y += sd.c
ramstage-y += sm.c
ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smi.c
ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smi_util.c
ramstage-y += usb.c

all-y += reset.c

smm-y += smihandler.c
smm-y += smi_util.c

CPPFLAGS_common += -I$(src)/southbridge/amd/pi/hudson/include

# ROMSIG At ROMBASE + 0x20000:
# +-----------+---------------+----------------+------------+
# |0x55AA55AA |EC ROM Address |GEC ROM Address |USB3 ROM    |
# +-----------+---------------+----------------+------------+
# |PSPDIR ADDR|
# +-----------+
#
# EC ROM should be 64K aligned.

ifeq ($(CONFIG_AMDFW_OUTSIDE_CBFS),y)
HUDSON_FWM_POSITION=0x20000
else
HUDSON_FWM_POSITION=0x720000
endif

add_opt_prefix=$(if $(call strip_quotes, $(1)), $(2) $(call strip_quotes, $(1)), )

OPT_HUDSON_XHCI_FWM_FILE=$(call add_opt_prefix, $(CONFIG_HUDSON_XHCI_FWM_FILE), --xhci)
OPT_HUDSON_IMC_FWM_FILE=$(call add_opt_prefix, $(CONFIG_HUDSON_IMC_FWM_FILE), --imc)
OPT_HUDSON_GEC_FWM_FILE=$(call add_opt_prefix, $(CONFIG_HUDSON_GEC_FWM_FILE), --gec)

$(obj)/amdfw.rom:	$(call strip_quotes, $(CONFIG_HUDSON_XHCI_FWM_FILE)) \
			$(call strip_quotes, $(CONFIG_HUDSON_IMC_FWM_FILE)) \
			$(call strip_quotes, $(CONFIG_HUDSON_GEC_FWM_FILE)) \
			$(DEP_FILES) \
			$(AMDFWTOOL)
	rm -f $@
	@printf "    AMDFWTOOL  $(subst $(obj)/,,$(@))\n"
	$(AMDFWTOOL) \
		$(OPT_HUDSON_XHCI_FWM_FILE) \
		$(OPT_HUDSON_IMC_FWM_FILE) \
		$(OPT_HUDSON_GEC_FWM_FILE) \
		--flashsize $(CONFIG_ROM_SIZE) \
		--location $(HUDSON_FWM_POSITION) \
		--config $(CONFIG_AMDFW_CONFIG_FILE) \
		--output	$@

ifeq ($(CONFIG_AMDFW_OUTSIDE_CBFS),y)
$(call add_intermediate, add_amdfw, $(obj)/amdfw.rom)
	printf "    DD         Adding AMD Firmware\n"
	dd if=$(obj)/amdfw.rom \
		of=$< conv=notrunc bs=1 seek=131072 >/dev/null 2>&1

else # ifeq ($(CONFIG_AMDFW_OUTSIDE_CBFS),y)

cbfs-files-y += apu/amdfw
apu/amdfw-file := $(obj)/amdfw.rom
apu/amdfw-position := $(HUDSON_FWM_POSITION)
apu/amdfw-type := raw

endif # ifeq ($(CONFIG_AMDFW_OUTSIDE_CBFS),y)
