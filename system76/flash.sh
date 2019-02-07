#!/usr/bin/env bash

set -ex

D="$(dirname "$(realpath "$0")")"

for N in "$@"
do
    P="$D/$N"
    if [ ! -d "$P" ]
    then
        echo "$0 [model]"
        exit 1
    fi

    sudo "$P/flashrom" \
        -p internal:laptop=force_I_want_a_brick \
        -w "$P/coreboot"
done
