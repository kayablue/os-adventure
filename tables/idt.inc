;------------------------------- Constants -----------------------------
                ; Basic PIC stuff
PIC1:           equ 0x20                ; Master PIC Base Address
PIC2:           equ 0xA0                ; Slave PIC Base Address
PIC1_COMMAND:   equ 0x20
PIC1_DATA:      equ 0x21
PIC2_COMMAND:   equ 0xA0
PIC2_DATA:      equ 0xA1

                ; Commands here
PIC_EOI:        equ 0x20                ; End of Interrupt Command

                ; Initialization Words
ICW1_INIT:      equ 0x10                ; Basic Init Command
ICW1_ICW4:      equ 0x01                ; If we need ICW4
ICW1_SINGLE:    equ 0x02                ; If we want Single (Not Cascaded) Mode
ICW1_INTERVAL:  equ 0x04                ; CALL address interval of 4 (not 8) (whatever that means)
ICW1_LEVEL:     equ 0x08                ; Level triggered mode (whatever that means)

ICW4_8086:      equ 0x01                ; If we want 8086 (MCS-80) mode

                ; PS/2 Controller Stuff
PS2_PORT:       equ 0x64
PS2_DATA:       equ 0x60
PS2_ACK:        equ 0xFA                

                ; PS/2 Controller Commands
PS2_GET_STATUS: equ 0x20                ; Get status (no ACK)
PS2_SET_STATUS: equ 0x60                ; Set status (yes ACK) ((but qemu doesn't do that))
PS2_MOUSE_ON:   equ 0xA8                ; Mouse on (yes ACK) ((but qemu doesn't do that))

                ; PS/2 Mouse Specific Commands
PS2_MOUSE_WRITE:                equ 0xD4        ; Use this to send commands to mouse
PS2_MOUSE_ENABLE_DATA:          equ 0xF4        ; Enable sending data packets
PS2_MOUSE_SET_SAMPLE_RATE:      equ 0xF3        ; Set sample rate (20 to 200 or something)
PS2_MOUSE_SET_SCALING_1:        equ 0xE6        ; Set scaling 1:1 (linear)

;---------------------------- Handler Imports --------------------------
        %include "interrupt_handlers/empty_handler.inc"
        %include "interrupt_handlers/pic_handler.inc"
        %include "interrupt_handlers/keyboard_handler.inc"
        %include "interrupt_handlers/mouse_handler.inc"


;------------------------------- IDT Stuff -----------------------------

        ; Creates n interrupt descriptors pointing to the empty handler
%macro set_unhandled 1
        %rep %1
                dw empty_handler
                dw CODE_SEGMENT
                db 0
                db 0b10001110
                dw 0
        %endrep

%endmacro

%macro set_handled 1
        int_%1_descriptor:
                dw 0
                dw CODE_SEGMENT
                db 0
                db 0b10001110
                dw 0
%endmacro


        ; Hell yeah, the IDT table is here
IDT_Start:
        set_unhandled 32                ; we'll not handle other interrupts (yet)

        set_handled 32
        set_handled 33

        set_unhandled 10

        set_handled 44

        set_unhandled 256 - 45
IDT_End:


IDT_Descriptor:
        dw IDT_Start - IDT_End - 1
        dd IDT_Start


                ; Puts the specified label into offset of specified IDT entry
%macro init_int 2
        mov eax, %2
        mov [int_%1_descriptor], ax
        shr eax, 16
        mov [int_%1_descriptor + 6], ax
%endmacro

initialize_interrupts:
        ; Initialize all known defined interrupts here
        init_int 32, pic_interrupt
        init_int 33, keyboard_interrupt
        init_int 44, mouse_interrupt


        ; Load that all in afterwards
        lidt [IDT_Descriptor]
        jmp CODE_SEGMENT:lidt_ready

lidt_ready:
        call configure_ps2
        call configure_pic
        sti
        ret

configure_pic:
        ; Configure the PIC here
        in al, PIC1_DATA                ; Save the masks
        mov ah, al
        in al, PIC2_DATA
        push ax

        mov al, ICW1_INIT | ICW1_ICW4   ; ICW1: initialization
        out PIC1_COMMAND, al
        call io_wait
        out PIC2_COMMAND, al
        call io_wait

        mov al, 0x20
        out PIC1_DATA, al               ; ICW2: Master Offset
        call io_wait
        mov al, 0x28
        out PIC2_DATA, al               ; ICW2: Slave Offset
        call io_wait

        mov al, 4
        out PIC1_DATA, al               ; ICW3: Specify Slave at IRQ 2
        call io_wait

        mov al, 2
        out PIC2_DATA, al               ; ICW3: Slave ID = 2
        call io_wait

        mov al, ICW4_8086               ; ICW4: 8086 mode
        out PIC1_DATA, al
        call io_wait
        out PIC2_DATA, al
        call io_wait

        pop ax

        xor ax, ax
        out PIC2_DATA, al               ; Setting back the interrupt masks
        call io_wait
        mov al, ah
        out PIC1_DATA, al
        call io_wait

        ret

configure_ps2:
        call ps2_out_wait
        mov al, PS2_GET_STATUS
        out PS2_PORT, al

        call ps2_in_wait
        in al, PS2_DATA
        mov bl, al
        or bl, 00000010b
        and bl, 11011111b

        call ps2_out_wait
        mov al, PS2_SET_STATUS
        out PS2_PORT, al

        call ps2_out_wait
        mov al, bl
        out PS2_DATA, al            

        call ps2_out_wait
        mov al, PS2_MOUSE_ON
        out PS2_PORT, al
        
        call ps2_out_wait
        mov al, PS2_MOUSE_WRITE
        out PS2_PORT, al

        call ps2_out_wait
        mov al, PS2_MOUSE_SET_SCALING_1
        out PS2_DATA, al
        call mouse_ack_wait

        call ps2_out_wait
        mov al, PS2_MOUSE_WRITE
        out PS2_PORT, al

        call ps2_out_wait
        mov al, PS2_MOUSE_SET_SAMPLE_RATE
        out PS2_DATA, al

        call mouse_ack_wait

        call ps2_out_wait
        mov al, PS2_MOUSE_WRITE
        out PS2_PORT, al

        call ps2_out_wait
        mov al, 200
        out PS2_DATA, al
        
        call mouse_ack_wait

        call ps2_out_wait
        mov al, PS2_MOUSE_WRITE
        out PS2_PORT, al

        call ps2_out_wait
        mov al, PS2_MOUSE_ENABLE_DATA
        out PS2_DATA, al
        
        call mouse_ack_wait


        ret

io_wait:
        push ax
        mov al, 0
        out 0x80, al
        pop ax
        ret

        ; ps2_out_wait() -> waits for bit 1 of ps/2 status port to be 0
ps2_out_wait:
        push ax
.loop:  
        in al, PS2_PORT
        test al, 1 << 1
        jnz .loop

        pop ax
        ret

        ; ps2_in_wait() -> waits for bit 0 of ps/2 status port to be 1
ps2_in_wait:
        push ax

.loop:  in al, PS2_PORT
        test al, 1
        jz .loop

        pop ax
        ret

ps2_ack_wait:
        push ax
.loop:   
        call ps2_in_wait


        in al, PS2_DATA
        cmp al, 0xFA
        jne .loop

        pop ax
        ret

mouse_ack_wait:
        push ax

.loop:
        in al, PS2_PORT
        and al, 00100001b
        cmp al, 00100001b
        jne .loop
        in al, PS2_DATA
        cmp al, 0xFA
        jne .loop

        pop ax
        ret
