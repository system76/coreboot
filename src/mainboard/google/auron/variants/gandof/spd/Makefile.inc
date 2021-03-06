## SPDX-License-Identifier: GPL-2.0-only

romstage-y += spd.c

# { GPIO47, GPIO9, GPIO13 }
SPD_SOURCES  = Samsung_M471B5674EB0-YK0			# 0b0000
SPD_SOURCES += Hynix_HMT425S6CFR6A_H5TC4G63CFR		# 0b0001
SPD_SOURCES += empty					# 0b0010
SPD_SOURCES += empty					# 0b0011
SPD_SOURCES += Samsung_M471B5674EB0-YK0			# 0b0100
SPD_SOURCES += Hynix_HMT425S6CFR6A_H5TC4G63CFR		# 0b0101
SPD_SOURCES += empty					# 0b0110
SPD_SOURCES += empty					# 0b0111

LIB_SPD_DEPS := $(foreach f, $(SPD_SOURCES), src/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/spd/$(f).spd.hex)
