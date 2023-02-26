@ECHO off
nasm -fbin boot.asm -o boot.bin
qemu-system-x86_64 -drive file=boot.bin,format=raw -monitor stdio