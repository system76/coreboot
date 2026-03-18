Upcoming release - coreboot 26.03
========================================================================

The coreboot project is pleased to announce the release of coreboot 26.03.
This release incorporates 856 commits from 95 contributors, reflecting
continued focus on runtime configuration, security hardening, platform
enablement, and memory/display data reliability across supported systems.

Key improvements include:
- Full support for Intel PantherLake SoCs (released at CES 2026 this January)
- Refinements to CFR-based runtime configuration, including clearer
  dependency wiring and more platform-exposed options
- Increased SMMSTORE capacity and improved handling of mmapped SMMSTORE reads
- TPM and signed secure blob plumbing updates across multiple platforms
- SPD/memory data generation refresh, including JSON-based sources and
  build-time `spd.hex` generation
- MediaTek MT8196 display pipeline improvements (DSI dual-channel support and
  panel power-off handling)
- Storage integrity and security hardening improvements, including COREBOOT/
  BOOTBLOCK locking and CBFS verification redundancy support

As always, the coreboot project extends our appreciation to everyone who
contributed to this release. From experienced developers implementing
hardware-focused platform changes to community members validating
functionality and reporting issues, the combined effort drives the
continued evolution of open firmware and more reliable system initialization.

The next coreboot release, 26.06, is scheduled for the end of June 2026.


Significant or interesting changes
----------------------------------

### drivers/option/cfr: Refine and extend runtime configuration options

The coreboot Forms Representation (CFR) runtime option framework was
refactored to make platform dependencies more explicit and to keep option
selection consistent across Intel SoCs and the boards that expose CFR-based
controls.

Intel SoC CFR integration was simplified to use boolean dependency wiring
instead of enum-based gating, and the option plumbing was cleaned up across
multiple boards and variants. In addition to general stability and
integration work, several platforms gained new CFR-exposed runtime options
for PCIe/power configuration, input device identification, Bluetooth runtime
configuration, and a number of firmware configuration controls used by
payloads that support CFR-driven setup.


### drivers/smmstore: Improve memory-mapped reads for SMMSTORE

The SMMSTORE driver was adjusted to use `lookup_store()` for
memory-mapped reads, improving consistency of mmapped access patterns across
architectures and reducing the likelihood of stale mappings during runtime
reads.


### TPM and signed secure blobs: board wiring and common plumbing improvements

TPM enablement received cross-platform updates that cover ACPI integration,
PSP emulation paths, and board-specific wiring.

ACPI received additional enums for the TPM2 start method. On Qualcomm
platforms, logic was added to handle secure boot blob paths in support of
signed secure blob flows (gated by new Kconfig for signed blobs). The
AMD/PSP software stack gained support for an fTPM driver implementation for
PSP emulated CRB TPMs.

Several board implementations were updated to match expected TPM resource
layouts, including moving TPM into the devicetree on Lenovo platforms and
adding TPM I2C plus EC SPI configuration on supported ChromeOS boards.


### Memory SPD generation refresh: JSON-based sources and LP5/LPDDR5 expandability

SPD-related work focused on reducing the amount of manual SPD data
maintenance while improving coverage for new memory parts.

Starlabs platform SPD sources were converted to JSON, and the build system
now generates `spd.hex` from these JSON inputs at build time. The SPD
tooling also gained improvements to `spd_gen` for Make-driven builds, along
with new SPD decode helpers used for newer LPDDR5/LP5 families.

Additional fixes in the SPD parsing pipeline address DDR5 channel mapping and
corner-case arithmetic issues, improving the reliability of generated memory
configuration.


### MediaTek MT8196 display pipeline updates (DSI dual channel, panel power-off)

The MediaTek MT8196 display stack was extended for broader DSI topologies and
more consistent panel power management.

DSI support was expanded to handle dual-channel configurations, with updated
DSI PHY control method integration. Panel command handling was extended with
explicit power-off support via a shared panel power-off API, and the
platform gained common low-battery power-off handling for consistent runtime
behavior.

These updates refine initialization sequencing and improve the timing
interaction between boot-state based initialization and display bring-up.


### Firmware security hardening: lockdown controls and CBFS redundancy verification

Security and integrity controls were expanded across multiple platforms.

Intel SoCs moved to a common PMC lockdown driver, and several platforms
received a shared runtime control path for BIOS SMM write protection.
coreboot also gained an option to lock COREBOOT and BOOTBLOCK contents,
strengthening the expected tamper resistance during normal operation.

On the storage integrity side, CBFS verification was extended with Top Swap
redundancy support, improving robustness when payload redundancy is used as
part of the platform update or recovery strategy.



Additional coreboot changes
---------------------------

The following are changes across a number of patches, or changes worth
noting, but not needing a full description.

* Intel SPI flash robustness improvements including spinlock protection for fast
  operations and prevention of transfers across 4KiB boundaries.
* Additional Intel SoC initialization refactoring, including common late-stage
  finalize implementation used by multiple x86 families.
* PCIe and FSP parameter updates across several Intel SoCs, including
  platform-specific root port programming and improved PCIe power management
  configuration.
* Panther Lake/WCL power management work, including additional power-state
  threshold configuration and PCIe clock-control support.
* ChromeOS platform boot/power robustness improvements, including secure
  low-power charging path handling and improved behavior when charger applet
  enablement fails.
* Display and camera pipeline fixes, including sensor SSDB validation and
  board-specific MIPI camera configuration updates where required.
* Starlabs platform storage/power enablement including helper-based NVMe
  power sequencing and additional card reader enablement controls.
* Qualcomm x1p42100 enablement updates including secure boot blob path logic,
  DMA/stack region adjustments, and SPI bus speed tuning.
* RISC-V SMP correctness fixes, including resolution of a race in SMP bring-up.



Changes to external resources
-----------------------------

### Toolchain updates

* Upgrade clang used by coreboot's cross toolchain build to 21.1.8.
* Cross-compilation fixes for toolchain selection and target arguments to
  keep the build deterministic across host environments.

### Git submodule pointers

* `amd_blobs` submodule pointer advanced for updated AMD firmware blobs.
* `libgfxinit` submodule bumped to a newer revision.
* `vboot` submodule updated from the 2024 snapshot to the upstream 2026
  baseline.

### External payloads

* EDK2 build now generates an optional update capsule in support of UEFI update
  flows. Payload configuration updates include a new MrChromebox branch baseline
  and serial-output adjustments.
* Depthcharge default configuration updated to use a 64-bit libpayload setup.
* libpayload extended to pass panel power-off commands to payloads, and
  liblz4 constant definitions were exported to keep payload-side compression
  logic in sync.

Platform Updates
----------------

### Added mainboards:
* Intel Pantherlake CRB
* Qotom qdnv01
* Siemens MC EHL7
* Siemens MC EHL8
* Star Labs Starbook Horizon


### Removed Mainboards


### Updated SoCs
* AMD Turin POC: Added SPI TPM SoC-specific initialization and aligned Turin
  sources for the PoC platform.
* Intel Alderlake: Dropped invalid builtin root port handling in FSP params,
  migrated to common global reset and common PMC lockdown plumbing, and
  cleaned up CFR dependency wiring.
* Intel Cannonlake: Migrated to common global reset and standardized common
  GSPI/I2C/UART device mapping plus shared CFR boolean wiring.
* Intel Elkhartlake: Switched to common global reset and common eSPI/LPC
  initialization with shared GSPI/I2C/UART devfn mappings.
* Intel Jasperlake: Added IGD naming via `soc_acpi_name`, moved PCIe ModPHY
  parameter wiring into shared pcie_rp.h, standardized common global
  reset/eSPI initialization, and added an initial `chipset.cb`.
* Intel Meteorlake: Switched to Arrow Lake FSP usage (including Arrow Lake
  microcode binaries) and migrated to shared finalize/global reset/PMC
  lockdown implementations, plus ARL-H 45W power entry support.
* Intel Pantherlake: Tuned WCL SKU behavior (icc_max and power-state
  thresholds, TdcTimeWindow defaults, and CD clock frequency control) while
  moving the platform to shared finalize/global reset/PMC lockdown flows.
* Intel Tigerlake: Migrated to common global reset and common PCH client
  SMI handling, standardized eSPI/LPC initialization plus shared
  SoundWire/SPI/I2C/UART device mapping, and replaced CFR enums with
  booleans.
* MediaTek MT8196: Expanded DSI to support dual-channel configurations,
  improved display pipeline bring-up (including framebuffer sizing and MIPI
  register control), and added support for dual-MIPI logo rendering and
  common panel power-off handling.
* Qualcomm x1p42100: Added secure boot blob path logic for signed secure
  blob support, increased SPI bus frequency (75MHz), and adjusted DMA/stack
  region layout; also extended LPASS-side APIs used for platform bring-up.


Statistics from the 25.12 to the 26.03 release
--------------------------------------------
* Total Commits: 856
* Average Commits per day: 9.42
* Total lines added: 107457
* Average lines added per commit: 125.53
* Number of patches adding more than 100 lines: 72
* Average lines added per small commit: 41.11
* Total lines removed: 22154
* Average lines removed per commit: 25.88
* Total difference between added and removed: 85303
* Total authors: 95



coreboot Links and Contact Information
--------------------------------------

* Main Web site: <https://www.coreboot.org>
* Downloads: <https://coreboot.org/downloads.html>
* Source control: <https://review.coreboot.org>
* Documentation: <https://doc.coreboot.org>
* Issue tracker: <https://ticket.coreboot.org/projects/coreboot>
* Donations: <https://coreboot.org/donate.html>
