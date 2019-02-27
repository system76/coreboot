#!/usr/bin/env bash

set -ex

MODEL="$(grep CONFIG_VARIANT_DIR .config | cut -d '"' -f2)"

time make -j$(nproc) BUILD_TIMELESS=1

cp -v build/coreboot.rom "system76/$MODEL/coreboot"
