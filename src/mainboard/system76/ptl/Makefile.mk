## SPDX-License-Identifier: GPL-2.0-only

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include

bootblock-y += bootblock.c
bootblock-y += variants/$(VARIANT_DIR)/gpio_early.c

romstage-y += variants/$(VARIANT_DIR)/romstage.c

ramstage-y += ramstage.c
ramstage-y += variants/$(VARIANT_DIR)/hda_verb.c
ramstage-y += variants/$(VARIANT_DIR)/gpio.c
ramstage-$(CONFIG_DRIVERS_I2C_TAS5825M) += variants/$(VARIANT_DIR)/tas5825m.c

SPD_SOURCES = Foresee-16GB-FX5P2004G-60 Foresee-32GB-FX5P4008G-60 Micron-16GB-MT62F1G32D2DS-023 Micron-32GB-MT62F2G32D4DS-023
