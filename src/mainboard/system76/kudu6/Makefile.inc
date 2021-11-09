# SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c
bootblock-y += early_gpio.c

romstage-y += port_descriptors.c

APCB_SOURCES = $(src)/mainboard/$(MAINBOARDDIR)/data.apcb
APCB_SOURCES_RECOVERY = $(src)/mainboard/$(MAINBOARDDIR)/data.apcb_recovery
APCB_SOURCES_68 = $(src)/mainboard/$(MAINBOARDDIR)/data.apcb_68
