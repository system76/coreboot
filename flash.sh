#!/usr/bin/env bash

set -e

MODEL="$(grep CONFIG_MAINBOARD_PART_NUMBER .config | cut -d '"' -f2)"

if [ "$MODEL" != "$(cat /sys/class/dmi/id/product_version)" ]
then
   echo "DMI does not match $MODEL"
   exit 1
fi

sudo "system76/$MODEL/flashrom" \
    -p internal:laptop=force_I_want_a_brick \
    -w "build/coreboot.rom"

