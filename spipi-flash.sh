#!/usr/bin/env bash

set -e

SPIPI="ubuntu@10.17.89.179"

MODEL="$(grep CONFIG_MAINBOARD_PART_NUMBER .config | cut -d '"' -f2)"
CHIP="$(cat "system76/$MODEL/chip")"

sftp "$SPIPI" <<EOF
cd firmware
put build/coreboot.rom "$MODEL.rom"
EOF

ssh -T "$SPIPI" <<EOF
cd firmware
./spipi.sh -c "$CHIP" -w "$MODEL.rom"
EOF
