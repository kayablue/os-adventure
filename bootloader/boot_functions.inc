        ; _convertWordToHex(ax: int, di: *output) -> puts contents of ax in hex into *output
_convertWordToHex:
        push ax

        add di, 2
        call _convertByteToHex

        mov al, ah
        sub di, 2
        call _convertByteToHex

        pop ax
        ret

        ; _convertByteToHex(al: int, di: *output) -> puts contents of al in hex into *output
_convertByteToHex:
        push cx
        push ax

        mov ah, al      ; save contents of al for further use

        mov cl, 4
        shr al, 4

        call _toHex
        mov byte[di], al

        mov al, ah
        and al, 0b00001111

        call _toHex
        mov byte[di + 1], al

        pop ax
        pop cx
        ret

        ; _convertByteToDecimal(al: int, di: *output) -> puts contents of al in decimal into *output
_convertByteToDecimal:
        push bx
        push cx
        push ax

        add di, 2
        mov cl, 10

.loop:  mov ah, 0
        div cl

        add ah, 48
        mov byte[di], ah
        dec di

        cmp al, 0
        jne .loop
        
        pop ax
        pop cx
        pop bx
        ret

        ; _toHex(al: 0 <= int < 16) -> al but in ASCII
_toHex:
        cmp al, 9
        ja .letter
.number sub al, 7
.letter add al, 55

        ret

        ; _printf(si: message*) -> output to console buffer
bios_print:
        push ax
        push bx
.loop:  lodsb

        cmp al, 0
        je .end
        mov ah, 0eh
        mov bh, 0
        int 10h
        jmp .loop

.end:   pop bx
        pop ax
        ret