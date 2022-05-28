// Syscall conventions
// Svc number: 0
// Syscall number: x8
// Return value: x0

.global syscall
syscall:
    mov x8, x0 
    svc #0
    ret
