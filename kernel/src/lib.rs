#![no_std]
#![no_main]

pub use timer;
pub use pl011;

use core::arch::asm;

#[no_mangle]
pub extern "C" fn drop_privilege() {
   unsafe {
       asm!(
            "msr spsr_el1, xzr"
       );
   }
}


#[no_mangle]
pub extern "C" fn get_lr() -> u64 {
   unsafe {
       let lr: u64;
       asm!("mov {}, x25", out(reg) lr);
       return lr;
   }
}

#[no_mangle]
pub extern "C" fn set_lr(lr: u64) {
    unsafe {
        asm!("mov x30, {}", in(reg) lr);
    };
}

#[no_mangle]
pub extern "C" fn get_sp_el0() -> u64 {
   unsafe {
       let addr: u64;
       asm!("mrs {}, sp_el0", out(reg) addr);
       return addr;
   }
}

#[no_mangle]
pub extern "C" fn set_sp_el0(addr: u64) {
    unsafe {
        asm!("msr sp_el0, {}", in(reg) addr);
    };
}

#[no_mangle]
pub extern "C" fn get_elr_el1() -> u64 {
   unsafe {
       let addr: u64;
       asm!("mrs {}, elr_el1", out(reg) addr);
       return addr;
   }
}

#[no_mangle]
pub extern "C" fn set_elr_el1(addr: u64) {
    unsafe {
        asm!("mov x22, {}", in(reg) addr);
        asm!("msr elr_el1, {}", in(reg) addr);
    };
}

#[no_mangle]
pub extern "C" fn exception_return() {
    unsafe {
        asm!("eret");
    }
}

#[no_mangle]
pub extern "C" fn mask_interrupts() {
    unsafe {
        asm!("msr daifset, #0b0011");
    }
}

#[no_mangle]
pub extern "C" fn unmask_interrupts() {
    unsafe {
        asm!("msr daifclr, #0b0011");
    }
}

#[panic_handler]
fn panic_handler(_info: &core::panic::PanicInfo) -> ! {
    loop {}
}
