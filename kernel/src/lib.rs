#![no_std]
#![no_main]

pub use pl011;
pub use timer;
pub mod exception;

#[panic_handler]
fn panic_handler(_info: &core::panic::PanicInfo) -> ! {
    loop {}
}
