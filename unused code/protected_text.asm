        mov dx, 3CCh            ; Set Miscellaneous Output Register 1st byte to 1
        in al, dx
        or al, 1

        sub dx, 10
        out dx, al

        mov dx, 3CCh
        in al, dx
        mov [debug_data], al


        call text_clear

        mov ax, 33 + 80 * 11
        call set_cursor_position

        mov esi, debug_msg
        mov edi, 66 + 80 * 2 * 11
        call text_print