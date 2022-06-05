
static mut HEAP_PTR: *mut u8 = 0 as *mut u8;
static mut HEAP_OFFSET: usize = 0;

#[no_mangle]
pub extern "C" fn kmalloc_init(addr: *mut u8) {
    unsafe {
        HEAP_PTR = addr;
    }
}

#[no_mangle]
pub extern "C" fn kmalloc(size: usize) -> *mut u8 {
    unsafe {
        let ptr: *mut u8 = HEAP_PTR.offset(HEAP_OFFSET as isize);
        HEAP_OFFSET += size;
        return ptr;
    }
}
