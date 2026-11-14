// SPDX-License-Identifier: GPL-2.0-only

// TODO: Provide compiler-builtins
#![expect(internal_features)]
#![feature(compiler_builtins)]
#![compiler_builtins]
#![no_builtins]

use core::{
    ffi::{
        c_void,
        c_char,
    },
    ptr,
};

// TODO: Move to coreboot library
#[derive(Debug)]
#[repr(C)]
pub struct Bus {
    pub dev: *mut Device,
    pub children: *mut Device,
    pub bridge_ctrl: u32,
    pub bridge_cmd: u16,
    pub secondary: u16,
    pub subordinate: u16,
    pub max_subordinate: u16,
    pub segment_group: u8,
    pub flags: u32,
}

// TODO: Move to coreboot library
#[derive(Debug)]
#[repr(C)]
pub struct Device {
    pub upstream: *mut Bus,
    pub downstream: *mut Bus,
    pub sibling: *mut Device,
    pub next: *mut Device,
    // path
    pub vendor: u32,
    pub device: u32,
    pub subsystem_vendor: u16,
    pub subsystem_device: u16,
    pub class: u32,
    pub hdr_type: u32,
    pub flags: u32,
    pub command: u8,
    pub hotplug_buses: u16,
    // resource_list
    // if !DEVTREE_EARLY
    pub ops: *mut DeviceOperations,
    pub chip_ops: *mut ChipOperations,
    pub name: *const c_char,
    pub pci_vga_option_rom: *mut c_void, // rom_header
    // ...
    pub chip_info: *mut c_void,
    // ...
}

// TODO: Move to coreboot library
#[derive(Debug)]
#[repr(C)]
pub struct DeviceOperations {
    pub read_resources: Option<extern "C" fn(dev: *mut Device)>,
    pub set_resources: Option<extern "C" fn(dev: *mut Device)>,
    pub enable_resources: Option<extern "C" fn(dev: *mut Device)>,
    pub init: Option<extern "C" fn(dev: *mut Device)>,
    pub r#final: Option<extern "C" fn(dev: *mut Device)>,
    pub scan_bus: Option<extern "C" fn(dev: *mut Device)>,
    pub enable: Option<extern "C" fn(dev: *mut Device)>,
    pub vga_disable: Option<extern "C" fn(dev: *mut Device)>,
    pub reset_bus: Option<extern "C" fn(bus: *mut Bus)>,
    // ...
}

// TODO: Move to coreboot library
impl DeviceOperations {
    const fn const_default() -> Self {
        Self {
            read_resources: None,
            set_resources: None,
            enable_resources: None,
            init: None,
            r#final: None,
            scan_bus: None,
            enable: None,
            vga_disable: None,
            reset_bus: None,
        }
    }
}

impl Default for DeviceOperations {
    fn default() -> Self {
        Self::const_default()
    }
}

// TODO: Move to coreboot library
#[derive(Debug)]
#[repr(C)]
pub struct ChipOperations {
    pub enable_dev: Option<extern "C" fn(dev: *mut Device)>,
    pub init: Option<extern "C" fn(chip_info: *mut c_void)>,
    pub r#final: Option<extern "C" fn(chip_info: *mut c_void)>,
    pub flags: u32,
    pub name: *const c_char,
}

impl ChipOperations {
    const fn const_default() -> Self {
        Self {
            enable_dev: None,
            init: None,
            r#final: None,
            flags: 0,
            name: ptr::null(),
        }
    }
}

impl Default for ChipOperations {
    fn default() -> Self {
        Self::const_default()
    }
}

// TODO: Move to coreboot library
#[unsafe(no_mangle)]
pub extern "C" fn noop_read_resources(_dev: *mut Device) {}
// TODO: Move to coreboot library
#[unsafe(no_mangle)]
pub extern "C" fn noop_set_resources(_dev: *mut Device) {}

#[unsafe(no_mangle)]
pub extern "C" fn tas5825m_write_at(_dev: *mut Device, _addr: u8, _value: u8) -> i32 {
    todo!()
}

#[unsafe(no_mangle)]
pub extern "C" fn tas5825m_write_block_at(_dev: *mut Device, _addr: u8, _values: *const u8, _length: u8) -> i32 {
    todo!()
}

#[unsafe(no_mangle)]
pub extern "C" fn tas5825m_set_page(_dev: *mut Device, _page: u8) -> i32 {
    todo!()
}

#[unsafe(no_mangle)]
pub extern "C" fn tas5825m_set_book(_dev: *mut Device, _book: u8) -> i32 {
    todo!()
}

extern "C" fn tas5825m_init(_dev: *mut Device) {
    todo!()
}

// TODO: macro? for declaring device ops
static mut TAS5825M_OPERATIONS: DeviceOperations = DeviceOperations {
    read_resources: Some(noop_read_resources),
    set_resources: Some(noop_set_resources),
    init: Some(tas5825m_init),
    ..DeviceOperations::const_default()
};

extern "C" fn tas5825m_enable_dev(dev: *mut Device) {
    unsafe { (*dev).ops = &raw mut TAS5825M_OPERATIONS; }
}

// TODO: macro? for declaring chip ops
#[unsafe(no_mangle)]
pub static mut drivers_i2c_tas5825m_ops: ChipOperations = ChipOperations {
    name: c"TI TAS5825M Amplifier".as_ptr(),
    enable_dev: Some(tas5825m_enable_dev),
    ..ChipOperations::const_default()
};

// TODO: Move to coreboot library
#[panic_handler]
fn panic(_info: &core::panic::PanicInfo) -> ! {
    loop {}
}
