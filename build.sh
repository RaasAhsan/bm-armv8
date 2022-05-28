#!/bin/bash

cargo build --release && \
as boot.s -o boot.o && \
as vector.s -o vector.o && \
gcc -Iinclude -ffreestanding -c kernel.c -o kernel.o && \
gcc -Iinclude -ffreestanding -c uart.c -o uart.o && \
gcc -Iinclude -ffreestanding -c gic.c -o gic.o && \
gcc -Iinclude -ffreestanding -c scheduler.c -o scheduler.o && \
gcc -Iinclude -ffreestanding -c syscall.c -o syscall.o && \
ld -nostdlib -T linker.ld boot.o vector.o kernel.o uart.o gic.o scheduler.o syscall.o target/aarch64-unknown-none/release/libkernel.a -o kernel.elf && \
objcopy -O binary kernel.elf kernel.bin && \
objdump -d kernel.elf > kernel.dump
