# SPDX-License-Identifier: GPL-2.0-or-later

bootblock-y += gpio.c
romstage-y += gpio.c
ramstage-y += gpio.c
verstage-y += gpio.c

subdirs-y += ./memory

romstage-y += variant.c
