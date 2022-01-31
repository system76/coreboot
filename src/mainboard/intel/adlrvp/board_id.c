/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>
#include <ec/acpi/ec.h>
#include <ec/google/chromeec/ec.h>
#include <types.h>
#include "board_id.h"

static uint32_t get_board_id_via_ext_ec(void)
{
	uint32_t id = BOARD_ID_INIT;

	if (google_chromeec_get_board_version(&id))
		id = BOARD_ID_UNKNOWN;

	return id;
}

/* Get Board ID via EC I/O port write/read */
int get_board_id(void)
{
	MAYBE_STATIC_NONZERO int id = -1;

	if (id < 0) {
		if (CONFIG(EC_GOOGLE_CHROMEEC)) {
			id = get_board_id_via_ext_ec();
		} else {
			if (send_ec_command(EC_FAB_ID_CMD) == 0) {
				id = recv_ec_data() << 8;
				id |= recv_ec_data();
			}
		}
	}
	return (id & BOARD_ID_MASK);
}
