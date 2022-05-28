#!/bin/bash

cargo build --release && \
as src/boot.s -o out/boot.o && \
as src/vector.s -o out/vector.o && \
as src/syscall.s -o out/syscall.o && \
gcc -Iinclude -ffreestanding -c src/kernel.c -o out/kernel.o && \
gcc -Iinclude -ffreestanding -c src/uart.c -o out/uart.o && \
gcc -Iinclude -ffreestanding -c src/gic.c -o out/gic.o && \
gcc -Iinclude -ffreestanding -c src/scheduler.c -o out/scheduler.o && \
gcc -Iinclude -ffreestanding -c src/process.c -o out/process.o && \
ld -nostdlib -T linker.ld out/boot.o out/vector.o out/kernel.o out/uart.o out/gic.o out/scheduler.o out/process.o out/syscall.o target/aarch64-unknown-none/release/libkernel.a -o out/kernel.elf && \
objcopy -O binary out/kernel.elf out/kernel.bin && \
objdump -d out/kernel.elf > out/kernel.dump
