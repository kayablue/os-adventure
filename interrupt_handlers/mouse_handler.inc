MOUSE_PORT: equ 0x60            ; yes, it's the same as keyboard port

mouse_interrupt:
        pushfd
        push ax
        push ebx
        push edx

        xor ebx, ebx
        xor edx, edx

        in al, MOUSE_PORT                       ; Take in byte
        xor ebx, ebx
        mov bl, [mouse_data.current_byte]
        mov [mouse_data.packet + ebx], al
        inc byte[mouse_data.current_byte]

        cmp byte[mouse_data.current_byte], 3             ; Draw data when we accumulated the whole packet
        jne .eoi

        ; Here is where we have the whole packet

        mov byte[mouse_data.current_byte], 0

        mov al, [mouse_data.packet]
        test al, 1 << 3                                 ; Test for validity
        jz .eoi
        test al, 11000000b                              ; Saem thingy
        jnz .eoi

        test al, 1                                      ; Test for left click !
        jz .recalculate_pos

        xor ebx, ebx
        xor edx, edx
        push ax
        mov al, 0x0F
        mov bx, [mouse_data.position]
        mov dx, [mouse_data.position + 2]
        call set_canvas_pixel
        pop ax

.recalculate_pos:
        call add_pos_x
        call add_pos_y


.eoi:
        mov al, PIC_EOI
        out PIC1_COMMAND, al
        out PIC2_COMMAND, al

        pop edx
        pop ebx
        pop ax
        popfd

        iret

add_pos_x:
        push bx
        xor bx, bx
        test al, 1 << 4
        jz .x_positive
        mov bh, 0xFF                            ; Sign extension

.x_positive:
        mov bl, [mouse_data.packet + 1]
        add [mouse_data.position], bx

        cmp word[mouse_data.position], 0
        jge .right_check

.out_of_bnds_left:
        mov word[mouse_data.position], 0
        jmp .bounds_normal
.right_check:
        cmp word[mouse_data.position], 320 - 1
        jle .bounds_normal

.out_of_bnds_right:
        mov word[mouse_data.position], 320 - 1

.bounds_normal:
        pop bx
        ret

add_pos_y:
        push bx
        xor bx, bx
        test al, 1 << 5
        jz .y_positive
        mov bh, 0xFF                            ; Sign extension

.y_positive:
        mov bl, [mouse_data.packet + 2]
        neg bx
        add [mouse_data.position + 2], bx

        cmp word[mouse_data.position + 2], 0
        jge .bottom_check

.out_of_bnds_top:
        mov word[mouse_data.position + 2], 0
        jmp .bounds_normal
.bottom_check:
        cmp word[mouse_data.position + 2], 200 - 1
        jle .bounds_normal

.out_of_bnds_bottom:
        mov word[mouse_data.position + 2], 200 - 1

.bounds_normal:
        pop bx
        ret


mouse_data:
        .packet:    db 0, 0, 0                          ; 3-byte packet data
        .current_byte: db 0                             ; To see if we've accumulated the whole packet
        .position: dw 100, 100                          ; X, Y coordinates of the mouse