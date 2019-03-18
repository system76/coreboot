#!/usr/bin/env bash

set -e

D="$(dirname "$(realpath "$0")")"

for N in "$@"
do
    P="$D/$N"
    if [ ! -d "$P" ]
    then
        echo "$0 [model]"
        exit 1
    fi

    kvm \
        -bios "$P/coreboot" \
        -M q35 \
        -m 2048 \
        -net nic,model=rtl8139 \
        -serial mon:stdio \
        -vga std
done
