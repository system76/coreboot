/* SPDX-License-Identifier: GPL-2.0-only */

#if defined(CONFIG_BOARD_SYSTEM76_DARP5)
	#define COLOR_KEYBOARD 1
#elif defined(CONFIG_BOARD_SYSTEM76_GALP3_C)
	#define COLOR_KEYBOARD 0
#else
	#error Unknown Mainboard
#endif

Scope (\_SB) {
	#include "ac.asl"
	#include "battery.asl"
	#include "buttons.asl"
	#include "hid.asl"
	#include "lid.asl"
	#include "s76.asl"
	#include "sleep.asl"
}

#include "tbt.asl"

Scope (_GPE) {
	#include "gpe.asl"
}
