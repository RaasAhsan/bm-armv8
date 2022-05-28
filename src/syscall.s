// Syscall conventions
// Svc number: 0
// Syscall number: x8
// arguments: x0, x1, x2, x3, x4, x5
// Return value: x0

.global syscall
syscall:
    mov x8, x0
    mov x0, x1
    mov x1, x2
    mov x2, x3
    mov x3, x4
    mov x4, x5
    mov x5, x6
    svc #0
    ret
