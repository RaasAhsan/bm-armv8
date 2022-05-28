#![no_std]
#![no_main]

pub use pl011;
pub use timer;
pub mod exception;
pub mod syscall;

use core::arch::asm;

#[no_mangle]
pub extern "C" fn drop_privilege() {
    unsafe {
        asm!("msr spsr_el1, xzr");
    }
}

#[panic_handler]
fn panic_handler(_info: &core::panic::PanicInfo) -> ! {
    loop {}
}
