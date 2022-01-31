/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio.h>
#include <amdblocks/lpc.h>
#include <amdblocks/pmlib.h>
#include <amdblocks/reset.h>
#include <amdblocks/smbus.h>
#include <amdblocks/spi.h>
#include <soc/southbridge.h>
#include <types.h>

static void sb_enable_lpc(void)
{
	u8 byte;

	/* Enable LPC controller */
	byte = pm_io_read8(PM_LPC_GATING);
	byte |= PM_LPC_ENABLE;
	pm_io_write8(PM_LPC_GATING, byte);
}

static void sb_lpc_decode(void)
{
	u32 tmp = 0;

	/* Enable I/O decode to LPC bus */
	tmp = DECODE_ENABLE_PARALLEL_PORT0 | DECODE_ENABLE_PARALLEL_PORT2
		| DECODE_ENABLE_PARALLEL_PORT4 | DECODE_ENABLE_SERIAL_PORT0
		| DECODE_ENABLE_SERIAL_PORT1 | DECODE_ENABLE_SERIAL_PORT2
		| DECODE_ENABLE_SERIAL_PORT3 | DECODE_ENABLE_SERIAL_PORT4
		| DECODE_ENABLE_SERIAL_PORT5 | DECODE_ENABLE_SERIAL_PORT6
		| DECODE_ENABLE_SERIAL_PORT7 | DECODE_ENABLE_AUDIO_PORT0
		| DECODE_ENABLE_AUDIO_PORT1 | DECODE_ENABLE_AUDIO_PORT2
		| DECODE_ENABLE_AUDIO_PORT3 | DECODE_ENABLE_MSS_PORT2
		| DECODE_ENABLE_MSS_PORT3 | DECODE_ENABLE_FDC_PORT0
		| DECODE_ENABLE_FDC_PORT1 | DECODE_ENABLE_GAME_PORT
		| DECODE_ENABLE_KBC_PORT | DECODE_ENABLE_ACPIUC_PORT
		| DECODE_ENABLE_ADLIB_PORT;

	/* Decode SIOs at 2E/2F and 4E/4F */
	if (CONFIG(STONEYRIDGE_LEGACY_FREE))
		tmp |= DECODE_ALTERNATE_SIO_ENABLE | DECODE_SIO_ENABLE;

	lpc_enable_decode(tmp);
}

static void setup_spread_spectrum(int *reboot)
{
	uint16_t rstcfg = pm_read16(PWR_RESET_CFG);

	rstcfg &= ~TOGGLE_ALL_PWR_GOOD;
	pm_write16(PWR_RESET_CFG, rstcfg);

	uint32_t cntl1 = misc_read32(MISC_CLK_CNTL1);

	if (cntl1 & CG1PLL_FBDIV_TEST) {
		printk(BIOS_DEBUG, "Spread spectrum is ready\n");
		misc_write32(MISC_CGPLL_CONFIG1,
			     misc_read32(MISC_CGPLL_CONFIG1) |
				     CG1PLL_SPREAD_SPECTRUM_ENABLE);

		return;
	}

	printk(BIOS_DEBUG, "Setting up spread spectrum\n");

	uint32_t cfg6 = misc_read32(MISC_CGPLL_CONFIG6);
	cfg6 &= ~CG1PLL_LF_MODE_MASK;
	cfg6 |= (0x0f8 << CG1PLL_LF_MODE_SHIFT) & CG1PLL_LF_MODE_MASK;
	misc_write32(MISC_CGPLL_CONFIG6, cfg6);

	uint32_t cfg3 = misc_read32(MISC_CGPLL_CONFIG3);
	cfg3 &= ~CG1PLL_REFDIV_MASK;
	cfg3 |= (0x003 << CG1PLL_REFDIV_SHIFT) & CG1PLL_REFDIV_MASK;
	cfg3 &= ~CG1PLL_FBDIV_MASK;
	cfg3 |= (0x04b << CG1PLL_FBDIV_SHIFT) & CG1PLL_FBDIV_MASK;
	misc_write32(MISC_CGPLL_CONFIG3, cfg3);

	uint32_t cfg5 = misc_read32(MISC_CGPLL_CONFIG5);
	cfg5 &= ~SS_AMOUNT_NFRAC_SLIP_MASK;
	cfg5 |= (0x2 << SS_AMOUNT_NFRAC_SLIP_SHIFT) & SS_AMOUNT_NFRAC_SLIP_MASK;
	misc_write32(MISC_CGPLL_CONFIG5, cfg5);

	uint32_t cfg4 = misc_read32(MISC_CGPLL_CONFIG4);
	cfg4 &= ~SS_AMOUNT_DSFRAC_MASK;
	cfg4 |= (0xd000 << SS_AMOUNT_DSFRAC_SHIFT) & SS_AMOUNT_DSFRAC_MASK;
	cfg4 &= ~SS_STEP_SIZE_DSFRAC_MASK;
	cfg4 |= (0x02d5 << SS_STEP_SIZE_DSFRAC_SHIFT)
						& SS_STEP_SIZE_DSFRAC_MASK;
	misc_write32(MISC_CGPLL_CONFIG4, cfg4);

	rstcfg |= TOGGLE_ALL_PWR_GOOD;
	pm_write16(PWR_RESET_CFG, rstcfg);

	cntl1 |= CG1PLL_FBDIV_TEST;
	misc_write32(MISC_CLK_CNTL1, cntl1);

	*reboot = 1;
}

static void setup_misc(int *reboot)
{
	/* Undocumented register */
	uint32_t reg = misc_read32(0x50);
	if (!(reg & BIT(16))) {
		reg |= BIT(16);

		misc_write32(0x50, reg);
		*reboot = 1;
	}
}

/* Before console init */
void bootblock_fch_early_init(void)
{
	int reboot = 0;

	/* Enable_acpimmio_decode_pm04 to enable the ACPIMMIO decode which is needed to access
	   the GPIO registers. */
	enable_acpimmio_decode_pm04();
	lpc_enable_rom();
	sb_enable_lpc();
	lpc_enable_port80();
	sb_lpc_decode();
	/* Make sure the base address is predictable */
	lpc_set_spibase(SPI_BASE_ADDRESS);
	fch_spi_early_init();
	fch_smbus_init();
	fch_enable_cf9_io();
	setup_spread_spectrum(&reboot);
	setup_misc(&reboot);

	if (reboot)
		warm_reset();

	fch_enable_legacy_io();
	enable_aoac_devices();

	/* disable the keyboard reset function before mainboard GPIO setup */
	if (CONFIG(DISABLE_KEYBOARD_RESET_PIN))
		fch_disable_kb_rst();
}

/* After console init */
void bootblock_fch_init(void)
{
	pm_set_power_failure_state();
	fch_print_pmxc0_status();
	show_spi_speeds_and_modes();
}

void fch_clk_output_48Mhz(u32 osc)
{
	u32 ctrl;

	/*
	 * Clear the disable for OSCOUT1 (signal typically named XnnM_25M_48M)
	 * or OSCOUT2 (USBCLK/25M_48M_OSC).  The frequency defaults to 48MHz.
	 */
	ctrl = misc_read32(MISC_CLK_CNTL1);

	switch (osc) {
	case 1:
		ctrl &= ~OSCOUT1_CLK_OUTPUT_ENB;
		break;
	case 2:
		ctrl &= ~OSCOUT2_CLK_OUTPUT_ENB;
		break;
	default:
		return; /* do nothing if invalid */
	}
	misc_write32(MISC_CLK_CNTL1, ctrl);
}
