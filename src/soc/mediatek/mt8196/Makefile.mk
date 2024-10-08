## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_SOC_MEDIATEK_MT8196),y)

all-y += ../common/gpio.c ../common/gpio_op.c gpio.c
all-$(CONFIG_SPI_FLASH) += spi.c
all-y += timer.c
all-y += ../common/uart.c

bootblock-y += bootblock.c
bootblock-y += ../common/mmu_operations.c

romstage-y += ../common/cbmem.c
romstage-y += emi.c

ramstage-y += emi.c
ramstage-y += soc.c

CPPFLAGS_common += -Isrc/soc/mediatek/mt8196/include
CPPFLAGS_common += -Isrc/soc/mediatek/common/include

$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.bin
	./util/mtkheader/gen-bl-img.py mt8196 sf $< $@

endif
