# StarBook Mk VI (Ryzen 7 5800U)

## Specs

- CPU (full processor specs available at <https://www.amd.com>)
    - AMD Ryzen 7 5800U (Cezanne)
- EC
    - ITE IT5570E (Star Labs Merlin EC)
    - Backlit keyboard, with standard PS/2 keycodes and SCI hotkeys
    - Battery
    - USB-C PD charger
    - Suspend / resume (S3)
- GPU
    - Integrated AMD Radeon Graphics
    - eDP internal display
    - HDMI video
    - USB-C DisplayPort video
- Memory
    - 2 x DDR4 SODIMM
- Networking
    - M.2 2230 WLAN (PCIe)
    - Bluetooth (internal USB)
- Sound
    - Conexant CX20632 (HDA)
    - Internal speakers
    - Internal microphone
    - Combined headphone / microphone 3.5-mm jack
    - HDMI audio
    - USB-C DisplayPort audio
- Storage
    - M.2 2280 NVMe SSD
    - M.2 SATA SSD
    - Micro-SD card reader
- USB
    - Camera (internal USB)
    - Fingerprint reader (internal USB)
    - USB 3.x Type-A ports (left + right)
    - USB Type-C port (USB + DisplayPort alt-mode)

## Building coreboot

Please follow the [Star Labs build instructions](common/building.md) to build coreboot, using `config.starlabs_starbook_czn` as config file.

### Preliminaries

Prior to building coreboot the following files are required:
* AMD Cezanne PSP/SMU firmware binaries (used by `src/mainboard/starlabs/cezanne/fw.cfg`)
* ITE Embedded Controller firmware (ec.bin)

If you enable `CONFIG_ADD_APCB_SOURCES`, additional APCB files are required (see `src/mainboard/starlabs/cezanne/Makefile.mk`).

The files listed below are optional:
- Splash screen image in Windows 3.1 BMP format (Logo.bmp)

These files exist in the correct location in the StarLabsLtd/blobs repo on GitHub which is used in place of the standard 3rdparty/blobs repo.

### Build

The following commands will build a working image:

```bash
make distclean
make defconfig KBUILD_DEFCONFIG=configs/config.starlabs_starbook_czn
make
```

## Flashing coreboot

```{eval-rst}
+---------------------+------------+
| Type                | Value      |
+=====================+============+
| Socketed flash      | no         |
+---------------------+------------+
| Vendor              | Winbond    |
+---------------------+------------+
| Model               | W25Q128JW  |
+---------------------+------------+
| Size                | 16 MiB     |
+---------------------+------------+
| Package             | SOIC-8     |
+---------------------+------------+
| Internal flashing   | yes        |
+---------------------+------------+
| External flashing   | yes        |
+---------------------+------------+
```

Please see [here](common/flashing.md) for instructions on how to flash with fwupd.
