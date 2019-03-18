#!/usr/bin/env bash

set -e

if [ ! -e "$1" ]
then
    echo "$0 [coreboot-collector output]"
    exit 1
fi

cat <<"EOF"
/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 System76
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 */

#ifndef MAINBOARD_GPIO_H
#define MAINBOARD_GPIO_H

#include <soc/gpe.h>
#include <soc/gpio.h>

#ifndef __ACPI__

/* Pad configuration in ramstage. */
static const struct pad_config gpio_table[] = {
EOF

grep ^GP "$1" | \
cut -d ' ' -f1,3,4 | \
sort -V | \
while read line
do
    parts=()
    for part in $line
    do
        parts+=("$part")
    done
    echo -e "\t_PAD_CFG_STRUCT(${parts[0]}, $(printf '0x%x' "$((${parts[1]} & 0xfffffffd))"), $(printf '0x%x' "$((${parts[2]} & 0xffffff00))")),"
done

cat <<"EOF"
};

#endif

#endif
EOF
