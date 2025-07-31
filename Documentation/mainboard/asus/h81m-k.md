# ASUS H81M-K

This page describes how to run coreboot on the [ASUS H81M-K].

## Required proprietary blobs

Please see [mrc.bin](../../northbridge/intel/haswell/mrc.bin.md).

## Building coreboot

A fully working image should be possible just by setting your MAC
address and obtaining the Haswell mrc. You can set the basic config
with the following commands. However, it is strongly advised to use
`make menuconfig` afterwards (or instead), so that you can see all of
the settings.

```bash
make distclean # Note: this will remove your current config, if it exists.
touch .config
./util/scripts/config --enable VENDOR_ASUS
./util/scripts/config --enable BOARD_ASUS_H81M_K
./util/scripts/config --enable HAVE_MRC
./util/scripts/config --set-str MRC_FILE "path/to/mrc.bin"
./util/scripts/config --set-str REALTEK_8168_MACADDRESS "xx:xx:xx:xx:xx:xx" # Fill this in!
make olddefconfig
```

If you don't plan on using coreboot's serial console to collect logs,
you might want to disable it at this point (`./util/scripts/config
--disable CONSOLE_SERIAL`). It should reduce the boot time by several
seconds. However, a more flexible method is to change the console log
level from within an OS using `util/nvramtool`, or with the `nvramcui`
payload, which requires enabling CMOS options.

Now, run `make` to build the coreboot image.

## Flashing coreboot

### Internal programming

The main SPI flash cannot be written internally from stock bios.
To install coreboot for the first time, the flash chip must be removed and
flashed with an external programmer; flashing in-circuit doesn't work.
The flash chip is socketed, so it's easy to remove and reflash. Thereafter,
coreboot can be updated internally using `flashrom -p internal`.

### External programming

The flash chip is a 8 MiB socketed DIP-8 chip. Specifically, it's a
GigaDevice GD25B64C, whose datasheet can be found [here][GD25B64C].
The chip is located to the bottom right-hand side of the board. For
a precise location, refer to section 1.2.3 (Motherboard Layout) of the
[board manual], where the chip is labelled "64Mb BIOS". Take note of
the chip's orientation, remove it from its socket, and flash it with
an external programmer. For reference, the notch in the chip should be
facing towards the bottom of the board.

## Known issues

- There is no automatic, OS-independent fan control. This is because
  the Super I/O hardware monitor can only obtain valid CPU temperature
  readings from the PECI agent, but the required driver doesn't exist
  in coreboot. The `coretemp` driver can still be used for accurate CPU
  temperature readings from an OS.

Please also see [Known issues with Haswell](../../northbridge/intel/haswell/known-issues.md).

## Working

- USB
- S3 suspend/resume
- Gigabit Ethernet
- Wake-on-LAN
- PCIe
- integrated graphics
- graphics init with libgfxinit
- Discrete GPU with VGA Option ROM
- SATA
- hardware monitor (see [Known issues](#known-issues))
- onboard audio
- PS/2 keyboard & mouse
- initialisation with Haswell mrc
- flashrom under coreboot
- Using `me_cleaner`
- Haswell NRI

## Untested

- parallel port
- EHCI debug
- infrared module
- serial port
- chassis intrusion header
- chassis speaker header
- front panel audio


## Technology

```{eval-rst}
+------------------+--------------------------------------------------+
| Northbridge      | :doc:`../../northbridge/intel/haswell/index`     |
+------------------+--------------------------------------------------+
| Southbridge      | Intel Lynx Point (H81)                           |
+------------------+--------------------------------------------------+
| CPU              | Intel Haswell (LGA1150)                          |
+------------------+--------------------------------------------------+
| Super I/O        | Nuvoton NCT5539D                                 |
+------------------+--------------------------------------------------+
| EC               | None                                             |
+------------------+--------------------------------------------------+
| Coprocessor      | Intel Management Engine                          |
+------------------+--------------------------------------------------+
```

[ASUS H81M-K]: https://www.asus.com/motherboards-components/motherboards/business/h81mk/
[GD25B64C]: https://www.gigadevice.com.cn/Public/Uploads/uploadfile/files/20220714/DS-00092-GD25B64C-Rev2.1.pdf
[flashrom]: https://flashrom.org/
[Board manual]: https://www.asus.com/motherboards-components/motherboards/business/h81mk/helpdesk_manual?model2Name=H81MK
