        ORG 0x7c00
        [BITS 16]
section .text
        jmp main
        %include "bootloader/boot_functions.inc"

main:   mov bx, 0              ; setting up flat memory model
        mov ds, bx
        mov ss, bx
        mov sp, 0x7bff

        mov bx, 0x07e0
        mov es, bx

        mov byte[BOOT_DISK], dl ; for later use

        mov ah, 02h             ; reading next code data from disk
        mov al, 128               ; number of sectors to read
        mov bx, 0
        mov ch, 0
        mov cl, 2
        mov dh, 0
        mov dl, byte[BOOT_DISK]

        int 13h
        jc generic_error

        mov bx, 0x17e0
        mov es, bx

        mov ah, 02h             ; reading next code data from disk
        mov al, 128               ; number of sectors to read
        mov bx, 0
        mov ch, 0
        mov cl, 1
        mov dh, 1
        mov dl, byte[BOOT_DISK]

        int 13h
        jc generic_error

        mov bx, 0
        mov es, bx

        mov ah, 0
        mov al, 13h
        int 10h

        cli                     ; Going into protected mode routine
        lgdt [GDT_Descriptor]
        mov eax, cr0
        or al, 1
        mov cr0, eax

        jmp CODE_SEGMENT:protected_main

        jmp $

generic_error:                  ; Generic error: ouputs contents of ax where
        mov di, err_al          ; ah is the error code and al is additional information
        call _convertByteToHex

        push ax
        mov al, ah
        mov di, errcode
        call _convertByteToHex
        pop ax

        mov si, errmsg
        call bios_print

.halt:  jmp .halt



errmsg:         db "An error occured! Error Code: "
errcode:        db 48, 48, "h", 13, 10
                db "Additionally, contents of al: "
err_al:         db 48, 48, "h", 13, 10, 0

BOOT_DISK:      equ 0

        %include "tables/gdt.inc"

                times 510 - ($ - $$) db 0
bootbytes:      db 0x55, 0xaa

        %include "kernel/main.inc"

