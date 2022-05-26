#![no_std]
#![no_main]

const UART_BASE: u64 = 0x09000000;

const UART_DR: *mut u8 = UART_BASE as *mut u8;
const UART_FR: *mut u32 = (UART_BASE + 0x18) as *mut u32;

// Flag register masks
const UART_FR_RXFE: u32 = 1 << 4; // receive fifo empty
const UART_FR_TXFF: u32 = 1 << 5; // transmit fifo full

#[no_mangle]
pub extern "C" fn uart_getchar() -> i8 {
    unsafe {
        while core::ptr::read_volatile(UART_FR) & UART_FR_RXFE > 0 {}
        core::ptr::read_volatile(UART_DR) as i8
    }
}

#[no_mangle]
pub extern "C" fn uart_putchar(c: i8) {
    unsafe {
        core::ptr::write_volatile(UART_DR, c as u8);
        while core::ptr::read_volatile(UART_FR) & UART_FR_TXFF > 0 {}
    }
}

#[panic_handler]
fn panic_handler(_info: &core::panic::PanicInfo) -> ! {
    loop {}
}
