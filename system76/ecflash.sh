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

    sudo "$D/ecflash" \
        "$P/ec"
done
