```
cd drivers && \
cargo build --release && 
cd ../ && \
as boot.s -o boot.o && \
gcc -ffreestanding -c kernel.c -o kernel.o && \
gcc -ffreestanding -c uart.c -o uart.o && \
gcc -ffreestanding -c gic.c -o gic.o && \
ld -nostdlib -T linker.ld boot.o kernel.o uart.o gic.o drivers/target/aarch64-unknown-none/release/libuart.a -o kernel.elf && \
objcopy -O binary kernel.elf kernel.bin && \
objdump -d kernel.elf  > kernel.dump
```

```
x /4096 0x08000000
x /256 0x08000000
```

### Timers
1. https://developer.arm.com/documentation/102379/0000/The-processor-timers
2. https://developer.arm.com/documentation/102374/0101/Data-processing---arithmetic-and-logic-operations

Each timer has three registers:
1. ctl: control register
2. cval: comparator value
3. tval: timer value

EL1 virtual timer registers has a prefix CNTV, so registers are:
1. CNTV_CTL_EL0, CNTV_CVAL_EL0, CNTV_TVAL_EL0