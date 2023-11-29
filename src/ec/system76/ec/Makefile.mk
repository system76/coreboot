## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_EC_SYSTEM76_EC),y)

all-y += system76_ec.c

ramstage-y += smbios.c
ramstage-$(CONFIG_EC_SYSTEM76_EC_LOCKDOWN) += lockdown.c
ramstage-y += usbc_mux.c

smm-$(CONFIG_DEBUG_SMI) += system76_ec.c

endif
