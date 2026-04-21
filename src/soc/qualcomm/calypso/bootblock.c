/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <soc/mmu.h>
#include <soc/qspi_common.h>
#include <soc/qupv3_config_common.h>

#define SPI_BUS_CLOCK_FREQ (CONFIG_SOC_SPI_FREQ_MHZ * MHz)

/*
 * Configure QSPI GPIO pins with frequency-dependent drive strength.
 *
 * This function initializes the Chip Select, Data, and Clock lines for the
 * QSPI interface. It dynamically adjusts the drive strength based on the
 * configured SPI frequency to ensure signal integrity.
 */
void qspi_configure_gpios(void)
{
	/*
	 * Default to 8mA for frequencies <= 50MHz. For high-speed operation
	 * (e.g., 75MHz or 100MHz), increase drive strength to 16mA to compensate
	 * for trace capacitance and ensure sharp signal transitions (rise/fall times).
	 */
	uint32_t drive_str = GPIO_8MA;

	if (CONFIG_SOC_SPI_FREQ_MHZ > 50)
		drive_str = GPIO_16MA;

	gpio_output(QSPI_CS, 1);

	gpio_configure(QSPI_DATA_0, GPIO_FUNC_QSPI_DATA_0,
		GPIO_NO_PULL, drive_str, GPIO_OUTPUT);

	gpio_configure(QSPI_DATA_1, GPIO_FUNC_QSPI_DATA_1,
		GPIO_NO_PULL, drive_str, GPIO_OUTPUT);

	gpio_configure(QSPI_CLK, GPIO_FUNC_QSPI_CLK,
		GPIO_NO_PULL, drive_str, GPIO_OUTPUT);
}

void bootblock_soc_early_init(void)
{
	if (!CONFIG(COMPRESS_BOOTBLOCK))
		soc_mmu_init();
}

void bootblock_soc_init(void)
{
	clock_init();

	quadspi_init(SPI_BUS_CLOCK_FREQ);
	qupv3_fw_init();
}
