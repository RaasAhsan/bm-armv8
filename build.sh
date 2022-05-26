#!/bin/bash

cd drivers && \
cargo build --release && 
cd ../ && \
as boot.s -o boot.o && \
as vector.s -o vector.o && \
gcc -ffreestanding -c kernel.c -o kernel.o && \
gcc -ffreestanding -c uart.c -o uart.o && \
gcc -ffreestanding -c gic.c -o gic.o && \
ld -nostdlib -T linker.ld boot.o vector.o kernel.o uart.o gic.o drivers/target/aarch64-unknown-none/release/libuart.a -o kernel.elf && \
objcopy -O binary kernel.elf kernel.bin && \
objdump -d kernel.elf  > kernel.dump
