#!/usr/bin/env bash

set -e

./util/scripts/config --disable PAYLOAD_SEABIOS
./util/scripts/config --enable PAYLOAD_GRUB2
make olddefconfig

make -j $(nproc)

./build/cbfstool build/coreboot.rom add -t raw -f grub/vga -n vgaroms/sgabios.bin
./build/cbfstool build/coreboot.rom add -t raw -f grub/grub.cfg -n etc/grub.cfg
./build/cbfstool build/coreboot.rom add-payload -f grub/seabios -n img/seabios
./build/cbfstool build/coreboot.rom add-payload -f grub/tianocore -n img/tianocore

./build/cbfstool build/coreboot.rom print
