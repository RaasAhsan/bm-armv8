const ESR_EL1_EC_SHIFT: u64 = 26;
const ESR_EL1_EC_MASK: u64 = 0b111111;
const ESR_EL1_SVC_IMM16_MASK: u64 = 0xffff;

use core::arch::asm;

#[no_mangle]
pub extern "C" fn get_exception_class() -> u8 {
    let reg = get_esr_el1();
    let ec = (reg >> ESR_EL1_EC_SHIFT) & ESR_EL1_EC_MASK;
    return ec as u8;
}

#[no_mangle]
pub extern "C" fn get_exception_svc_value() -> u16 {
    let reg = get_esr_el1();
    let ec = reg & ESR_EL1_SVC_IMM16_MASK;
    return ec as u16;
}

#[no_mangle]
pub extern "C" fn set_exception_link(addr: u64) {
    unsafe {
        asm!("msr elr_el1, {}", in(reg) addr);
    }
}

#[no_mangle]
pub extern "C" fn return_from_exception() {
    unsafe {
        asm!("eret");
    }
}

fn get_esr_el1() -> u64 {
    unsafe {
        let esr_el1: u64;
        asm!("mrs {}, esr_el1", out(reg) esr_el1);
        return esr_el1;
    }
}
