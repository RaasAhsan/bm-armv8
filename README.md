```
as boot.s -o boot.o && \
gcc -ffreestanding -c kernel.c -o kernel.o && \
gcc -ffreestanding -c uart.c -o uart.o && \
gcc -ffreestanding -c gic.c -o gic.o && \
ld -nostdlib -T linker.ld boot.o kernel.o uart.o gic.o -o kernel.elf && \
objcopy -O binary kernel.elf kernel.bin
```