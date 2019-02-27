#!/usr/bin/env bash

set -ex

MODEL="$(grep CONFIG_VARIANT_DIR .config | cut -d '"' -f2)"

sudo "system76/$MODEL/flashrom" \
    -p internal:laptop=force_I_want_a_brick \
    -w "build/coreboot.rom"

