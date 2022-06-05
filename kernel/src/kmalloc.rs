// Implements a kernel memory heap via a bump allocator

struct BumpAllocator {
    base: usize,
    current: usize,
}

static mut ALLOC: BumpAllocator = BumpAllocator {
    base: 0,
    current: 0,
};

#[no_mangle]
pub extern "C" fn kmalloc_init(addr: *mut u8, size: usize) {
    unsafe {
        ALLOC.base = addr as usize;
        ALLOC.current = ALLOC.base + size;
    }
}

#[no_mangle]
pub extern "C" fn kmalloc(size: usize) -> *mut u8 {
    unsafe {
        // TODO: underflow
        let next = ALLOC.current - size;
        let remainder = next % size;
        let aligned_next = next - remainder;
        if aligned_next >= ALLOC.base {
            ALLOC.current = aligned_next;
            aligned_next as *mut u8
        } else {
            core::ptr::null_mut()
        }
    }
}
