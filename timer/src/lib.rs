#![no_std]
#![no_main]

use core::arch::asm;

const MICROS_PER_SECOND: u64 = 1000000;

#[no_mangle]
pub extern "C" fn timer_sleep(micros: u64) {
    let freq = get_timer_frequency();
    let start = get_system_count();
    loop {
        let curr = get_system_count();
        // Associativity: multiply by MICROS_PER_SECOND first to preserved precision
        let elapsed_micros = (curr - start) * MICROS_PER_SECOND / freq;
        if elapsed_micros >= micros {
            break;
        }
    }
}

#[no_mangle]
pub extern "C" fn timer_reset() {
    let freq = get_timer_frequency();
    set_virtual_timer_value(freq);
}

// TODO: volatile implied ?

fn get_timer_frequency() -> u64 {
    unsafe {
        let freq: u64;
        asm!("mrs {0}, cntfrq_el0", out(reg) freq);
        return freq;
    }
}

fn get_system_count() -> u64 {
    unsafe {
        let time: u64;
        asm!("mrs {0}, cntpct_el0", out(reg) time);
        return time;
    }
}

fn set_virtual_timer_value(count: u64) {
    unsafe {
        asm!("msr cntv_tval_el0, {}", in(reg) count);
    }
}
