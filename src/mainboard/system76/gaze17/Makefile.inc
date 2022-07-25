## SPDX-License-Identifier: GPL-2.0-only

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include

bootblock-y += bootblock.c
bootblock-y += variants/$(VARIANT_DIR)/gpio_early.c

romstage-y += romstage.c

ramstage-y += ramstage.c
ramstage-y += variants/$(VARIANT_DIR)/gpio.c
ramstage-y += variants/$(VARIANT_DIR)/hda_verb.c
