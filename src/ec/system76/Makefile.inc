ifeq ($(CONFIG_CONSOLE_SYSTEM76),y)

bootblock-y += system76_ec.c
verstage-y += system76_ec.c
romstage-y += system76_ec.c
postcar-y += system76_ec.c
ramstage-y += system76_ec.c
smm-$(CONFIG_DEBUG_SMI) += system76_ec.c

endif
