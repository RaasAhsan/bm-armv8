#![no_std]
#![no_main]

const UART_ADDRESS: u64 = 0x09000000;
const UART: *mut u8 = UART_ADDRESS as *mut u8;

#[no_mangle]
pub extern "C" fn register_driver() -> u8 {
    3
}

#[panic_handler]
fn panic_handler(_info: &core::panic::PanicInfo) -> ! {
    loop {}
}
