KEYBOARD_PORT:  equ 0x60

keyboard_interrupt:
        push ax
        push ebx
        push edx
 
        in al, KEYBOARD_PORT
        ; Do something with whatever the hell is received
        
        

        mov al, PIC_EOI
        out PIC1_COMMAND, al

        pop edx
        pop ebx
        pop ax
        iret

kbd_test:
        xor ebx, ebx
        mov edx, 20
        mov bl, al
        call set_canvas_pixel
        inc edx
        call set_canvas_pixel
        inc edx
        call set_canvas_pixel

        ret