# SPDX-License-Identifier: GPL-2.0-only

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include

bootblock-y += bootblock.c
bootblock-y += gpio_early.c

romstage-y += romstage.c
romstage-y += variants/$(VARIANT_DIR)/gpio.c

ramstage-y += ramstage.c
ramstage-y += variants/${VARIANT_DIR}/hda_verb.c
ramstage-y += variants/$(VARIANT_DIR)/gpio.c

ramstage-$(CONFIG_HAVE_ACPI_TABLES) += fadt.c
