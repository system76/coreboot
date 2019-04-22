#!/usr/bin/env bash

set -ex

MODEL="$(grep CONFIG_MAINBOARD_PART_NUMBER .config | cut -d '"' -f2)"
if [ "$MODEL" == "QEMU x86 q35/ich9" ]
then
    MODEL="qemu"
fi

time make -j$(nproc) BUILD_TIMELESS=1

cp -v build/coreboot.rom "system76/$MODEL/coreboot"
