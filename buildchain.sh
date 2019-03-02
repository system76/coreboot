#!/usr/bin/env bash

set -e
shopt -s nullglob

export LC=C.UTF-8
#export SOURCE_DATE_EPOCH="$(git log -1 --format='%ct')"

function usage {
    echo "buildchain.sh [destination]" >&2
}

function msg {
    echo -e "\e[1m""$@""\e[0m" >&2
}

if [ -z "$1" ]
then
    usage
    exit 1
fi
DIR="$(realpath "$1")"

mkdir -p "$DIR"

make crossgcc-i386 crossgcc-x64 CPUS="$(nproc)"

for model_dir in system76/*/
do
    model="$(basename "$model_dir")"

    echo -e "\x1B[1m$model\x1B[0m"
    make distclean
    cp -v "$model_dir/config" .config
    make olddefconfig
    time make -j "$(nproc)"
    mv -v "build/coreboot.rom" "$DIR/$model.rom"
done
