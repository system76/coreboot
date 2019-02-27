#!/usr/bin/env bash

set -e

export BUILD_TIMELESS=1

D="$(dirname "$(realpath "$0")")"

for N in "$@"
do
    P="$D/$N"
    if [ ! -d "$P" ]
    then
        echo "$0 [model]"
        exit 1
    fi

    # Ensure a clean build
    make distclean

    # Copy motherboard configuration
    cp -v "$P/config" ".config"

    # Compile coreboot
    make oldconfig
    time make -j$(nproc)

    # Copy coreboot file to product directory
    cp -v build/coreboot.rom "$P/coreboot"
done
