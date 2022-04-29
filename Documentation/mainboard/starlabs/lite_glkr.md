# StarLite Mk III

## Specs
- CPU (full processor specs available at https://ark.intel.com)
    - Intel N5030 (Gemini Lake Refresh)
- EC
    - Nuvoton NPCE985P/G
    - Backlit Keyboard, with standard PS/2 keycodes and SCI hotkeys
    - Battery
    - Charger, using AC adapter or USB-C PD
    - Suspend / resume
- GPU
    - Intel UHD Graphics 605
    - GOP driver is recommended, VBT is provided
    - eDP 11.6-inch 1920x1080 LCD
    - HDMI video
    - USB-C DisplayPort video
- Memory
    - 8GB on-board
- Networking
    - 9461 CNVi WiFi / Bluetooth soldered to PCBA
- Sound
    - Realtek ALC269
    - Internal speakers
    - Internal microphone
    - Combined headphone / microphone 3.5-mm jack
    - HDMI audio
    - USB-C DisplayPort audio
- Storage
    - M.2 SATA SSD
    - RTS5129 MicroSD card reader
- USB
    - 1200x1600 CCD camera
    - USB 3.1 Gen 1 Type-C (left)
    - USB 3.1 Gen 1 Type-A (left)
    - USB 3.1 Gen 1 Type-A (right)

## Building coreboot

### Preliminaries

Prior to building coreboot the following files are required:
* Intel Flash Descriptor file (descriptor.bin)
* IFWI Image (ifwi.rom)

The files listed below are optional:
- Splash screen image in Windows 3.1 BMP format (Logo.bmp)

These files exist in the correct location in the StarLabsLtd/blobs repo on GitHub which is used in place of the standard 3rdparty/blobs repo.

### Build

The following commands will build a working image:

```bash
make distclean
make defconfig KBUILD_DEFCONFIG=configs/config.starlabs_lite_glkr
make
```

## Flashing coreboot

```eval_rst
+---------------------+------------+
| Type                | Value      |
+=====================+============+
| Socketed flash      | no         |
+---------------------+------------+
| Vendor              | Gigadevice |
+---------------------+------------+
| Model               | GD25LQ64(B)|
+---------------------+------------+
| Size                | 8 MiB      |
+---------------------+------------+
| Package             | SOIC-8     |
+---------------------+------------+
| Internal flashing   | yes        |
+---------------------+------------+
| External flashing   | yes        |
+---------------------+------------+

Please see [here](../common/flashing.md) for instructions on how to flash with fwupd.
