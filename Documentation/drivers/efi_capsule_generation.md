# Generating signed UEFI capsules with EDK2

coreboot can cooperate with an EDK2 payload to support firmware updates via the UEFI
ESRT/FMP capsule mechanism.

This document covers generating a *signed* capsule during the coreboot build.

At present, capsule generation requires a compatible EDK2 tree with the
corresponding payload-side changes. Upstream support is being tracked in:

https://github.com/tianocore/edk2/pull/12053

Older EDK2 trees may be missing pieces required by this integration.

## Build-time capsule generation

Enable capsule support and use an EDK2 payload:

- `CONFIG_DRIVERS_EFI_UPDATE_CAPSULES`: enable coreboot capsule update support.
- `CONFIG_DRIVERS_EFI_GENERATE_CAPSULE`: generate `build/coreboot.cap` after the ROM is finalised.
- `CONFIG_PAYLOAD_EDK2`: build an EDK2 payload.

When enabled, the coreboot build generates `build/coreboot.cap` after the ROM image is
finalised. The capsule can also be generated explicitly with `make capsule`.

Configure the FMAP allowlist embedded into the ROM as a manifest:

- `CONFIG_DRIVERS_EFI_CAPSULE_REGIONS`: whitespace-separated FMAP region allowlist embedded into
  the ROM as a manifest (e.g. `COREBOOT EC`).

Configure the ESRT/FMP firmware identity used by the capsule:

- `CONFIG_DRIVERS_EFI_MAIN_FW_GUID`: GUID of the firmware
- `CONFIG_DRIVERS_EFI_MAIN_FW_VERSION`: firmware version encoded in the capsule header;
  if set to `0`, derive a value from the leading `<major>.<minor>` in
  `CONFIG_LOCALVERSION` when possible
- `CONFIG_DRIVERS_EFI_MAIN_FW_LSV`: lowest supported firmware version; if set to `0`,
  use the resolved firmware version

Reset behavior during capsule application:

- `CONFIG_DRIVERS_EFI_CAPSULE_INITIATE_RESET`: add the capsule `InitiateReset` flag.
  This is disabled by default because Linux rejects capsules with `InitiateReset` when using
  `/dev/efi_capsule_loader`.

## Embedded drivers (FmpDxe in capsule)

Some EDK2 capsule update flows use an embedded `FmpDxe.efi` driver inside the capsule.

To generate capsules with an embedded `FmpDxe.efi`, enable:

- `CONFIG_DRIVERS_EFI_CAPSULE_EMBED_FMP_DXE`: embed `FmpDxe.efi` into generated capsules.
- `CONFIG_DRIVERS_EFI_CAPSULE_ACCEPT_EMBEDDED_DRIVERS`: configure the EDK2 payload to accept
  capsules with embedded drivers (sets `PcdCapsuleEmbeddedDriverSupport=TRUE`).

Note: if Secure Boot is enabled, the embedded driver must be signed by a key trusted by the
running firmware, otherwise capsule processing may fail when loading the embedded driver.

## Capsule signing certificates

`GenerateCapsule` can sign the FMP payload (PKCS#7). Many platforms require signed capsules.

coreboot exposes three Kconfig options for the certificate chain:

- `CONFIG_DRIVERS_EFI_CAPSULE_SIGNER_PRIVATE_CERT`: PEM containing the signing private key and
  leaf certificate
- `CONFIG_DRIVERS_EFI_CAPSULE_OTHER_PUBLIC_CERT`: PEM intermediate certificate
- `CONFIG_DRIVERS_EFI_CAPSULE_TRUSTED_PUBLIC_CERT`: PEM trusted root certificate

If a configured path is relative, it is interpreted relative to the configured EDK2 repository
inside `payloads/external/edk2/workspace`.

The defaults use the EDK2 BaseTools test certificate chain. Do not use the test keys for
production firmware updates.

To generate your own certificate chain and convert it into the required PEM files, see:
`BaseTools/Source/Python/Pkcs7Sign/Readme.md` in the EDK2 tree.
