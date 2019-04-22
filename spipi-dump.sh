#!/usr/bin/env bash

set -e

SPIPI="ubuntu@10.17.89.179"

MODEL="$(grep CONFIG_MAINBOARD_PART_NUMBER .config | cut -d '"' -f2)"
CHIP="$(cat "system76/$MODEL/chip")"

ssh -T "$SPIPI" <<EOF
cd firmware
./spipi.sh -c "$CHIP" -r dump.rom
EOF

sftp "$SPIPI" <<EOF
cd firmware
get dump.rom build/dump.rom
EOF

./build/cbfstool build/dump.rom read -r CONSOLE -f build/dump.log

less build/dump.log
