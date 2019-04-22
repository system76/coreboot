#!/usr/bin/env bash

set -e

SPIPI="ubuntu@10.17.89.179"

MODEL="$(grep CONFIG_MAINBOARD_PART_NUMBER .config | cut -d '"' -f2)"
CHIP="$(cat "system76/$MODEL/chip")"

ssh -T "$SPIPI" <<EOF
cd firmware
./spipi.sh -c "$CHIP" -E
EOF
