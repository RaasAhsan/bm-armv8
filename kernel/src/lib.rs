#![no_std]
#![no_main]

pub use timer;
pub use pl011;

#[panic_handler]
fn panic_handler(_info: &core::panic::PanicInfo) -> ! {
    loop {}
}
