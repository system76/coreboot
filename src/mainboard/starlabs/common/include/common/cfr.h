/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _STARLABS_CMN_CFR_H_
#define _STARLABS_CMN_CFR_H_

#include <drivers/option/cfr_frontend.h>
#include <soc/soc_chip.h>
#include <common/powercap.h>
#include <common/touchpad.h>

void cfr_card_reader_update(struct sm_object *new_obj);
void cfr_touchscreen_update(struct sm_object *new_obj);
void starlabs_cfr_register_overrides(void);

static const struct sm_object accelerometer = SM_DECLARE_BOOL({
	.opt_name	= "accelerometer",
	.ui_name	= "Accelerometer",
	.ui_helptext	= "Enable or disable the built-in accelerometer",
	.default_value	= true,
});

static const struct sm_object bluetooth = SM_DECLARE_BOOL({
	.opt_name	= "bluetooth",
	.ui_name	= "Bluetooth",
	.ui_helptext	= "Enable or disable the built-in Bluetooth",
	.default_value	= true,
});

static const struct sm_object bluetooth_rtd3 = SM_DECLARE_BOOL({
	.opt_name	= "bluetooth_rtd3",
	.ui_name	= "Bluetooth Runtime-D3",
	.ui_helptext	= "Enable or disable Bluetooth power optimization.\n"
			  "Recommended to disable when booting Windows.",
	.default_value	= false,
});

static const struct sm_object card_reader = SM_DECLARE_BOOL({
	.opt_name	= "card_reader",
	.ui_name	= "Card Reader",
	.ui_helptext	= "Enable or disable the built-in card reader",
	.default_value	= !CONFIG(BOARD_STARLABS_LITE_ADL),
}, WITH_CALLBACK(cfr_card_reader_update));

static const struct sm_object fingerprint_reader = SM_DECLARE_BOOL({
	.opt_name	= "fingerprint_reader",
	.ui_name	= "Fingerprint Reader",
	.ui_helptext	= "Enable or disable the built-in fingerprint reader",
	.default_value	= true,
});

static const struct sm_object gna = SM_DECLARE_BOOL({
	.opt_name	= "gna",
	.ui_name	= "Gaussian & Neural Accelerator",
	.ui_helptext	= "Enable or Disable the Gaussian & Neural Accelerator",
	.default_value	= false,
});

static const struct sm_object hyper_threading = SM_DECLARE_BOOL({
	.opt_name	= "hyper_threading",
	.ui_name	= "Hyper-Threading",
	.ui_helptext	= "Enable or disable Hyper-Threading",
	.default_value	= true,
});

static const struct sm_object memory_speed = SM_DECLARE_ENUM({
	.opt_name	= "memory_speed",
	.ui_name	= "Memory Speed",
	.ui_helptext	= "Configure the speed that the memory will run at. "
			  "Higher speeds produce more heat and consume more power "
			  "but provide higher performance.",
	.default_value	= 1,
	.values		= (const struct sm_enum_value[]) {
		{ "5500MT/s", 0 },
		{ "6400MT/s", 1 },
		{ "7500MT/s", 2 },
		SM_ENUM_VALUE_END,
	},
});

static const struct sm_object microphone = SM_DECLARE_BOOL({
	.opt_name	= "microphone",
	.ui_name	= "Microphone",
	.ui_helptext	= "Enable or disable the built-in microphone",
	.default_value	= true,
});

static const struct sm_object power_profile = SM_DECLARE_ENUM({
	.opt_name	= "power_profile",
	.ui_name	= "Power Profile",
	.ui_helptext	= "Select whether to maximize performance, battery life or both.",
	.default_value	= PP_PERFORMANCE,
	.values		= (const struct sm_enum_value[]) {
			{ "Power Saver",	PP_POWER_SAVER	},
			{ "Balanced",		PP_BALANCED	},
			{ "Performance",	PP_PERFORMANCE	},
			SM_ENUM_VALUE_END			},
});

static const struct sm_object s0ix_enable = SM_DECLARE_BOOL({
	.opt_name	= "s0ix_enable",
	.ui_name	= "Modern Standby (S0ix)",
	.ui_helptext	= "Enabled: Use S0ix for device sleep.\n"
			  "Disabled: Use ACPI S3 for device sleep.\n"
			  "Requires Intel ME to be enabled.",
	.default_value	= false,
});

static const struct sm_object thunderbolt = SM_DECLARE_BOOL({
	.opt_name	= "thunderbolt",
	.ui_name	= "Thunderbolt",
	.ui_helptext	= "Enable or disable Thunderbolt support",
	.default_value	= true,
});

static const struct sm_object touchpad_haptics = SM_DECLARE_ENUM({
	.opt_name	= "touchpad_haptics",
	.ui_name	= "Touchpad Vibration Intensity",
	.ui_helptext	= "Choose how strong the touchpad click vibration feels.",
	.default_value	= STARLABS_TOUCHPAD_HAPTICS_DEFAULT,
	.values		= (const struct sm_enum_value[]) {
		{ "Off",	0 },
		{ "Low",	1 },
		{ "Medium",	2 },
		{ "High",	3 },
		{ "Maximum",	4 },
		SM_ENUM_VALUE_END,
	},
});

static const struct sm_object touchpad_force_press = SM_DECLARE_ENUM({
	.opt_name	= "touchpad_force_press",
	.ui_name	= "Touchpad Click Force",
	.ui_helptext	= "Choose how much force it takes to click the touchpad.",
	.default_value	= STARLABS_TOUCHPAD_PRESS_FORCE_DEFAULT,
	.values		= (const struct sm_enum_value[]) {
		{ "Minimal",	STARLABS_TOUCHPAD_FORCE_MINIMAL },
		{ "Low",	STARLABS_TOUCHPAD_FORCE_LOW },
		{ "Average",	STARLABS_TOUCHPAD_FORCE_AVERAGE },
		{ "High",	STARLABS_TOUCHPAD_FORCE_HIGH },
		{ "Hulk",	STARLABS_TOUCHPAD_FORCE_HULK },
		SM_ENUM_VALUE_END,
	},
});

static const struct sm_object touchpad_force_release = SM_DECLARE_ENUM({
	.opt_name	= "touchpad_force_release",
	.ui_name	= "Touchpad Release Force",
	.ui_helptext	= "Choose how much force it takes for the touchpad click to release.",
	.default_value	= STARLABS_TOUCHPAD_RELEASE_FORCE_DEFAULT,
	.values		= (const struct sm_enum_value[]) {
		{ "Minimal",	STARLABS_TOUCHPAD_FORCE_MINIMAL },
		{ "Low",	STARLABS_TOUCHPAD_FORCE_LOW },
		{ "Average",	STARLABS_TOUCHPAD_FORCE_AVERAGE },
		{ "High",	STARLABS_TOUCHPAD_FORCE_HIGH },
		{ "Hulk",	STARLABS_TOUCHPAD_FORCE_HULK },
		SM_ENUM_VALUE_END,
	},
});

static const struct sm_object touchpad_report_rate = SM_DECLARE_ENUM({
	.opt_name	= "touchpad_report_rate",
	.ui_name	= "Touchpad Tracking Speed",
	.ui_helptext	= "Choose how quickly the touchpad reports movement.",
	.default_value	= STARLABS_TOUCHPAD_REPORT_RATE_DEFAULT,
	.values		= (const struct sm_enum_value[]) {
		{ "Relaxed",	STARLABS_TOUCHPAD_RATE_RELAXED },
		{ "Balanced",	STARLABS_TOUCHPAD_RATE_BALANCED },
		{ "Fast",	STARLABS_TOUCHPAD_RATE_FAST },
		{ "Ludicrous",	STARLABS_TOUCHPAD_RATE_LUDICROUS },
		{ "Plaid",	STARLABS_TOUCHPAD_RATE_PLAID },
		SM_ENUM_VALUE_END,
	},
});

static const struct sm_object touchscreen = SM_DECLARE_BOOL({
	.opt_name	= "touchscreen",
	.ui_name	= "Touchscreen",
	.ui_helptext	= "Enable or disable the built-in touch-screen",
	.default_value	= true,
}, WITH_CALLBACK(cfr_touchscreen_update));

static const struct sm_object vpu = SM_DECLARE_BOOL({
	.opt_name	= "vpu",
	.ui_name	= "VPU",
	.ui_helptext	= "Enable or disable VPU",
	.default_value	= false,
});

static const struct sm_object vtd = SM_DECLARE_BOOL({
	.opt_name	= "vtd",
	.ui_name	= "VT-d",
	.ui_helptext	= "Enable or disable Intel VT-d (virtualization)",
	.default_value	= true,
});

static const struct sm_object webcam = SM_DECLARE_BOOL({
	.opt_name	= "webcam",
	.ui_name	= "Webcam",
	.ui_helptext	= "Enable or disable the built-in webcam",
	.default_value	= true,
});

static const struct sm_object wifi = SM_DECLARE_BOOL({
	.opt_name	= "wifi",
	.ui_name	= "WiFi",
	.ui_helptext	= "Enable or disable the built-in WiFi",
	.default_value	= true,
});

#endif /* _STARLABS_CMN_CFR_H_ */
