CANVAS_WIDTH:   equ 320

set_canvas_pixel:
        pushfd
        pushad

        mov edi, [canvas_ptr]
        add edi, ebx            ; add X
        
        mov bl, al
        mov eax, edx

        mov dx, CANVAS_WIDTH       ; Y *= 320
        mul dx

        add edi, eax            ; add Y

        mov [edi], bl

        popad
        popfd

        ret

canvas_ptr:     dd 0x0010FA00    ; Pointer to Canvas place in memory (it's in higher memory btw)