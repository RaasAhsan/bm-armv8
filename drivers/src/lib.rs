#![no_std]
#![no_main]

const UART_BASE: u64 = 0x09000000;

const UART_DR: *mut u8 = UART_BASE as *mut u8;
const UART_FR: *mut u32 = (UART_BASE + 0x18) as *mut u32;
const UART_CR: *mut u32 = (UART_BASE + 0x30) as *mut u32;
const UART_IMSC: *mut u32 = (UART_BASE + 0x38) as *mut u32;
const UART_ICR: *mut u32 = (UART_BASE + 0x44) as *mut u32;

// Flag register masks
const UART_FR_TXFE: u32 = 1 << 7; // transmit fifo empty
const UART_FR_RXFE: u32 = 1 << 4; // receive fifo empty
const UART_FR_TXFF: u32 = 1 << 5; // transmit fifo full

const UART_ICR_ALL: u32 = 0x7ff;


// IMSC register bits
const UART_IMSC_RTIM: u32 = 1 << 6;
const UART_IMSC_RXIM: u32 = 1 << 4;

// CR regitser bits
const UART_CR_RXE: u32 = 1 << 9;
const UART_CR_TXE: u32 = 1 << 8;
const UART_CR_UARTEN: u32 = 1 << 0;

fn uart_flush() {
    unsafe {
        while core::ptr::read_volatile(UART_FR) & UART_FR_TXFE == 0 {}
    }
}

#[no_mangle]
pub extern "C" fn uart_init() {
    unsafe {
        core::ptr::write_volatile(UART_CR, 0x00);
        core::ptr::write_volatile(UART_IMSC, UART_IMSC_RXIM | UART_IMSC_RTIM);
        core::ptr::write_volatile(UART_CR, UART_CR_UARTEN | UART_CR_RXE | UART_CR_TXE);
        uart_flush();
    }
}

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

#[no_mangle]
pub extern "C" fn uart_clear_interrupts() {
    unsafe {
        core::ptr::write_volatile(UART_ICR, UART_ICR_ALL);
    }
}

#[panic_handler]
fn panic_handler(_info: &core::panic::PanicInfo) -> ! {
    loop {}
}
