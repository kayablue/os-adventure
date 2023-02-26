; ------------------------------ Constants -----------------------------
CRTC_ADDRESS:   equ 0x03D4
CRTC_DATA:      equ 0x03D5
TEXT_BUFFER:    equ 0xB8000

;-----------------------------------------------------------------------
;-------------------------- Text Mode Functions ------------------------
;----------------------------------------------------------------------- 

        ; text_clear() -> simply clears the whole screen for text mode
text_clear:
        pushf
        push edi
        push eax

        mov al, " "
        mov ah, 0x0f

        xor edi, edi

.loop:        
        cmp edi, 2000
        je .end

        mov [TEXT_BUFFER + edi * 2], ax

        inc edi
        jmp .loop

.end:   
        pop eax
        pop edi
        popf
        ret

        ; text_print(esi: *source, edi: *offset) -> copies source string to [0xB8000 + di] 
text_print:
        push eax
        push esi
        push edi
        
        add edi, TEXT_BUFFER    ; edi pointing at video memory now

.loop:
        lodsb                   ; load string byte to al
        or al, 0                ; cmp if it's zero
        jz .end

        mov ah, 0x0F            ; White colour for now
        stosw                   ; store string byte from al

        jmp .loop               ; repeat
.end:
        pop edi
        pop esi
        pop eax
        ret 

        ; disable_cursor() -> disables text mode cursor
disable_cursor:
        push edx
        push eax

        mov dx, CRTC_ADDRESS    ; Select Cursor Start Register
        mov al, 0ah
        out dx, al

        mov dx, CRTC_DATA       ; Switch to CRTC Data Register
        in al, dx
        or al, 00100000b        ; Set Cursor Disable to 1
        out dx, al        

        pop eax
        pop edx
        ret

        ; enable_cursor() -> enables text mode cursor
enable_cursor:
        push edx
        push eax

        mov dx, CRTC_ADDRESS     
        mov al, 0ah             ; Cursor Start Register Index
        out dx, al

        mov dx, CRTC_DATA       ; Switch to CRTC Data Register
        in al, dx
        and al, 11011111b       ; Set Cursor Disable Bit to 1
        out dx, al        

        pop eax
        pop edx
        ret

        ; set_cursor_position(ax: position) -> Sets cursor to specified position
set_cursor_position:
        push eax
        push ebx

        mov bx, ax              ; For later use
        mov dx, CRTC_ADDRESS
        mov al, 0eh             ; Cursor Location High Register Index
        out dx, al

        mov dx, CRTC_DATA       ; Write Higher Byte
        mov al, bh
        out dx, al

        mov dx, CRTC_ADDRESS    
        mov al, 0fh             ; Cursor Location Low
        out dx, al 

        mov dx, CRTC_DATA
        mov al, bl              ; Write Lower Byte
        out dx, al       

        pop ebx
        pop eax
        ret