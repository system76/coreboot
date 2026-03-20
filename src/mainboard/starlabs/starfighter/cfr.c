/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <console/cfr.h>
#include <drivers/option/cfr_frontend.h>
#include <ec/starlabs/merlin/cfr.h>
#include <intelblocks/cfr.h>
#include <variants.h>
#include <common/cfr.h>

#if CONFIG(BOARD_STARLABS_STARFIGHTER_MTL)
static const struct sm_object legacy_speaker_control = SM_DECLARE_BOOL({
	.opt_name	= "legacy_speaker_control",
	.ui_name	= "Legacy Speaker Control",
	.ui_helptext	= "Enabled: keep the default speaker initialization.\n"
			  "Disabled: boot with GPIO2 low and LINE2 EAPD off "
			  "so the speakers start muted.",
	.default_value	= true,
});

static struct sm_obj_form audio_group = {
	.ui_name = "Audio",
	.obj_list = (const struct sm_object *[]) {
		&legacy_speaker_control,
		NULL
	},
};
#endif

static struct sm_obj_form battery_group = {
	.ui_name = "Battery",
	.obj_list = (const struct sm_object *[]) {
		#if CONFIG(EC_STARLABS_CHARGING_SPEED)
		&charging_speed,
		#endif
		&max_charge,
		&power_on_after_fail_bool,
		NULL
	},
};

static struct sm_obj_form debug_group = {
	.ui_name = "Debug",
	.obj_list = (const struct sm_object *[]) {
		&debug_level,
		NULL
	},
};

#if CONFIG(DRIVERS_INTEL_USB4_RETIMER)
static struct sm_obj_form io_expansion_group = {
	.ui_name = "I/O / Expansion",
	.obj_list = (const struct sm_object *[]) {
		&card_reader,
		&thunderbolt,
		NULL
	},
};
#endif

static struct sm_obj_form keyboard_group = {
	.ui_name = "Keyboard",
	.obj_list = (const struct sm_object *[]) {
		&fn_ctrl_swap,
		&kbl_timeout,
		NULL
	},
};

static struct sm_obj_form trackpad_group = {
	.ui_name = "Trackpad",
	.obj_list = (const struct sm_object *[]) {
		&touchpad_haptics,
		&touchpad_force_press,
		&touchpad_force_release,
		&touchpad_report_rate,
		NULL
	},
};

static struct sm_obj_form leds_group = {
	.ui_name = "LEDs",
	.obj_list = (const struct sm_object *[]) {
		&charge_led,
		&power_led,
		NULL
	},
};

static struct sm_obj_form pcie_power_management_group = {
	.ui_name = "PCIe Power Management",
	.obj_list = (const struct sm_object *[]) {
		#if CONFIG(SOC_INTEL_COMMON_BLOCK_ASPM)
		&pciexp_aspm,
		#if CONFIG(HAS_INTEL_CPU_ROOT_PORTS)
		&pciexp_aspm_cpu,
		#endif
		&pciexp_clk_pm,
		&pciexp_l1ss,
		#endif
		NULL
	},
};

static struct sm_obj_form performance_group = {
	.ui_name = "Performance",
	.obj_list = (const struct sm_object *[]) {
		&fan_mode,
		#if CONFIG(SOC_INTEL_TIGERLAKE) || CONFIG(SOC_INTEL_ALDERLAKE) || CONFIG(SOC_INTEL_RAPTORLAKE)
		&gna,
		#endif
		&hyper_threading,
		&memory_speed,
		&power_profile,
		#if CONFIG(SOC_INTEL_METEORLAKE)
		&vpu,
		#endif
		NULL
	},
};

static struct sm_obj_form security_group = {
	.ui_name = "Security",
	.obj_list = (const struct sm_object *[]) {
		&bios_lock,
		&intel_tme,
		&me_state,
		&me_state_counter,
		NULL
	},
};

static struct sm_obj_form suspend_lid_group = {
	.ui_name = "Suspend & Lid",
	.obj_list = (const struct sm_object *[]) {
		#if CONFIG(EC_STARLABS_LID_SWITCH)
		&lid_switch,
		#endif
		&s0ix_enable,
		NULL
	},
};

static struct sm_obj_form virtualization_group = {
	.ui_name = "Virtualization",
	.obj_list = (const struct sm_object *[]) {
		&vtd,
		NULL
	},
};

static struct sm_obj_form wireless_group = {
	.ui_name = "Wireless",
	.obj_list = (const struct sm_object *[]) {
		&bluetooth,
		&bluetooth_rtd3,
		&wifi,
		NULL
	},
};

static struct sm_obj_form *sm_root[] = {
	#if CONFIG(BOARD_STARLABS_STARFIGHTER_MTL)
	&audio_group,
	#endif
	&battery_group,
	&debug_group,
	#if CONFIG(DRIVERS_INTEL_USB4_RETIMER)
	&io_expansion_group,
	#endif
	&keyboard_group,
	&trackpad_group,
	&leds_group,
	&pcie_power_management_group,
	&performance_group,
	&security_group,
	&suspend_lid_group,
	&virtualization_group,
	&wireless_group,
	NULL
};

void mb_cfr_setup_menu(struct lb_cfr *cfr_root)
{
	starlabs_cfr_register_overrides();
	cfr_write_setup_menu(cfr_root, sm_root);
}
