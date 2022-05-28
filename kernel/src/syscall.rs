use core::arch::asm;

#[no_mangle]
pub extern "C" fn svc_syscall(number: u64) -> i64 {
    unsafe {
        let ret: i64;
        asm!("mov x8, {}", "svc #0", "mov {}, x0", in(reg) number, out(reg) ret);
        return ret;
    }
}
