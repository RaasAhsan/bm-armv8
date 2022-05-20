```
as boot.s -o boot.o && \
gcc -ffreestanding -c kernel.c -o kernel.o && \
gcc -ffreestanding -c uart.c -o uart.o && \
ld -nostdlib -T linker.ld boot.o kernel.o uart.o -o kernel.elf && \
objcopy -O binary kernel.elf kernel.bin
```