        ; 320 x 200 x 256
        xor edi, edi 
        xor al, al
        mov ecx, 80             ; 320 / 4

.col:
        push ecx
        push edi

        mov ecx, 200
.row:   mov [0xA0000 + edi], al
        mov [0xA0000 + edi + 1], al
        mov [0xA0000 + edi + 2], al
        mov [0xA0000 + edi + 3], al

        add edi, 320
        loop .row

        pop edi
        pop ecx

        add edi, 4
        inc al
        loop .col