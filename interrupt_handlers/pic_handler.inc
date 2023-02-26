pic_interrupt:
        push ax
        push edx
        push ebx

.eoi:
        mov al, PIC_EOI
        out PIC1_COMMAND, al
        pop ebx
        pop edx
        pop ax
        iret

pic_coord:      dd 0 
pic_color:      db 0

pic_test:
        xor edx, edx
        xor ebx, ebx

.stripe:
        mov edx, 170
        mov ebx, [pic_coord]
        mov al, [pic_color]
        call set_canvas_pixel

        inc edx
        call set_canvas_pixel

        inc edx
        call set_canvas_pixel

        inc dword[pic_coord]
        inc byte[pic_color]

        cmp dword[pic_coord], 320
        jb .end

        mov dword[pic_coord], 0

.end    ret