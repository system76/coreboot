/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <soc/romstage.h>

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	variant_memory_init(mupd);
}
