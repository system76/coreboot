## SPDX-License-Identifier: GPL-2.0-only

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include

bootblock-y += bootblock.c
bootblock-y += gpio_early.c

ramstage-y += ramstage.c
ramstage-y += gpio.c
ramstage-y += hda_verb.c
ramstage-y += tas5825m.c
ramstage-$(CONFIG_DRIVERS_OPTION_CFR) += cfr.c
