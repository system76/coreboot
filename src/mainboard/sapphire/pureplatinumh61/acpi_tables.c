/* This file is part of the coreboot project. */
/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <southbridge/intel/bd82x6x/nvs.h>

void acpi_create_gnvs(global_nvs_t *gnvs)
{
	gnvs->tcrt = 100;
	gnvs->tpsv = 90;
}
