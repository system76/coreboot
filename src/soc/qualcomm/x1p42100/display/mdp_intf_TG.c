/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/clock.h>
#include <soc/display/mdssreg.h>
#include <console/console.h>
#include <soc/qcom_spmi.h>
#include <gpio.h>

#define MDSS_MDP_MAX_PREFILL_FETCH 24

void intf_tg_setup(struct edid *edid)
{
	const struct edid_mode *m;
	uint32_t full_h_total, full_h_start, full_h_pw;
	uint32_t h_total, h_start, h_pw, h_end;
	uint32_t v_total, v_start, v_pw;

	uint32_t num_pipes = (edid->mode.ha > MDSS_MAX_SINGLE_PIPE_PIXEL_WIDTH) ? 2 : 1;

	if (!edid || !num_pipes)
		return;

	m = &edid->mode;
	if (!m->ha || !m->va)
		return;

	/* Full (non-split) timing from EDID */
	full_h_total = m->ha + m->hbl;
	full_h_start = m->hbl;
	full_h_pw = m->hspw;

	v_total = m->va + m->vbl;
	v_start = m->vbl;
	v_pw = m->vspw;

	/* Per-pipe horizontal timing */
	h_total = full_h_total / num_pipes;
	h_start = full_h_start / num_pipes;
	h_pw = full_h_pw / num_pipes;
	h_end = h_total - 1;

	write32(&mdp_intf->intf_mux, 0xF0000); // use ping_pong 0 & disable split

	write32(&mdp_intf->intf_hsync_ctl, (h_total << 16) | (h_pw));

	write32(&mdp_intf->intf_vysnc_period_f0, v_total * h_total);

	write32(&mdp_intf->intf_vysnc_pulse_width_f0, v_pw * h_total);

	write32(&mdp_intf->intf_disp_v_start_f0, v_start * h_total);

	write32(&mdp_intf->intf_disp_v_end_f0,
		(uint32_t)(((uint64_t)v_total * (uint64_t)h_total) - 1ULL));

	/* same packed value written to both regs (as before) */
	uint32_t disp_hctl = (h_end << 16) | (h_start);
	write32(&mdp_intf->intf_disp_hctl, disp_hctl);
	write32(&mdp_intf->display_data_hctl, disp_hctl);

	write32(&mdp_intf->polarity_ctl, (m->phsync ? 0x1 : 0x0) | (m->pvsync ? 0x2 : 0x0));

	write32(&mdp_intf->intf_panel_format, 0x2100); // Color Format : RGB
	write32(&mdp_intf->intf_prof_fetch_start, 0);
}

void intf_fetch_start_config(struct edid *edid)
{
	uint32_t v_total, h_total, fetch_start, vfp_start;
	uint32_t prefetch_avail, prefetch_needed;
	uint32_t fetch_enable = PROG_FETCH_START_EN;

	uint32_t num_pipes = (edid->mode.ha > MDSS_MAX_SINGLE_PIPE_PIXEL_WIDTH) ? 2 : 1;

	v_total = edid->mode.va + edid->mode.vbl;

	/* Per-pipe horizontal total (match TG programming) */
	if (((edid->mode.ha + edid->mode.hbl) % num_pipes) != 0)
		return;
	h_total = (edid->mode.ha + edid->mode.hbl) / num_pipes;

	vfp_start = edid->mode.va + edid->mode.vbl - edid->mode.vso;

	prefetch_avail = v_total - vfp_start;

	if (prefetch_avail >= MDSS_MDP_MAX_PREFILL_FETCH) {
		fetch_start = 0;
		fetch_enable = 0;
	} else {
		prefetch_needed = MDSS_MDP_MAX_PREFILL_FETCH;
		fetch_start = (v_total - prefetch_needed) * h_total + h_total + 1;
		fetch_enable = PROG_FETCH_START_EN;
	}

	write32(&mdp_intf->intf_prof_fetch_start, fetch_start);
	write32(&mdp_intf->intf_config, fetch_enable);
}
