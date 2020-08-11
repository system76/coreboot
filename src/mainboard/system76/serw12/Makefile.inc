# SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c
bootblock-y += early_gpio.c

ramstage-y += gpio.c
ramstage-y += port_descriptors.c

APCB_SOURCES = apcb

build/APCB_apcb.bin:
	cp -v $(FIRMWARE_OPEN_MODEL_DIR)/apcb.rom $@

