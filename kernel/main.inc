        [BITS 32]
        %include "tables/idt.inc"

        %include "kernel/graphics/canvas.inc"
        %include "kernel/graphics/text_functions.inc"
        %include "kernel/graphics/vga_functions.inc"


protected_main:
        call initialize_interrupts

.draw   call draw_screen
.halt   jmp .draw


debug_msg:      db "Debug Message: "
debug_data:     db "",  0
                times 1024 * 100 dd 0