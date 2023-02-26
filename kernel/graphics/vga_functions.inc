; ------------------------------ Constants -----------------------------
VGA_WIDTH:  equ 320
VGA_HEIGHT: equ 200
VGA_BUFFER: equ 0xA0000

;-----------------------------------------------------------------------
;---------------------- VGA 320x200 Mode Functions ---------------------
;----------------------------------------------------------------------- 


        ; set_pixel(ebx: x, edx: y, al: colour) -> Changes colour of pixel at coordinates (x, y)
set_pixel:
        pushfd
        pushad

        mov edi, [second_buffer_ptr]
        add edi, ebx            ; add X
        
        mov bl, al
        mov eax, edx

        mov dx, VGA_WIDTH       ; Y *= 320
        mul dx

        add edi, eax            ; add Y

        mov [edi], bl

        popad
        popfd

        ret

        ; draw_screen() -> Draws to the second buffer and then swaps buffers when
draw_screen:
        pushfd
        pushad

        ; Copy from canvas
        mov ecx, 320 * 200              
        mov esi, [canvas_ptr]
        mov edi, [second_buffer_ptr]
        rep movsb

        ; Draw cursor on top of canvas
        call draw_cursor

        cli                                     ; No interrupts until we're done refreshing the buffer 
        mov ecx, 320 * 200
        mov esi, [second_buffer_ptr]            ; Load the pointer
        mov edi, VGA_BUFFER
        rep movsb
        sti                                     ; Yes interrupts after we're done refreshing the buffer

        popad
        popfd
        ret

draw_cursor:
        pushad
        pushfd
        xor ebx, ebx
        xor edx, edx
        mov bx, [mouse_data.position]           ; Load in current position as draw start position
        mov dx, [mouse_data.position + 2]

        sub bx, 3                               ; Shift the bitmap start position for the cursor to be centered
        sub dx, 3

        xor ecx, ecx
        xor edi, edi
        mov cl, [cursor_len]
.bmp_rows:
        mov al, [cursor_bitmap + edi]           ; Take rows
        cmp dx, 0
        jl .next_row
        cmp dx, 200 - 1
        jg .next_row

        push ecx
        
        mov ecx, 8

.bmp_bits:
        push ecx

        dec cl
        mov ah, 1 
        shl ah, cl

        test al, ah
        jz .next_bit

        cmp bx, 0
        jl .next_bit
        cmp bx, 320 - 1
        jg .next_bit

        push ax
        
        mov al, 0x0F
        call set_pixel

        pop ax
        
.next_bit: 
        pop ecx

        inc bx
        loop .bmp_bits

        sub bx, 8

        pop ecx

.next_row:
        inc dx                                  ; Goin to next row                          
        inc edi
        loop .bmp_rows

        popfd
        popad
        ret

cursor_bitmap:          db 00010000b
                        db 00010000b
                        db 00000000b
                        db 11000110b
                        db 00000000b
                        db 00010000b
                        db 00010000b
cursor_len:             db 7

second_buffer_ptr:      dd 0x00100000   ; Second buffer for double buffering
