align 16
GDT_Start:
        null_descriptor:
                dd 0, 0
        code_descriptor:
                dw 0xFFFF       ; Limit (0 - 15)
                dw 0            ; Base (0 - 15)
                db 0            ; Base (16 - 23)
                db 0b10011010   ; P = 1, DPL = 0, S = 1, E = 1, C = 0, R = 1, A = 0
                db 0b11001111   ; G = 1, DB = 1, L = 0, Limit (16 - 19)
                db 0            ; Base (24 - 31)
        data_descriptor:
                dw 0xFFFF       ; Limit
                dw 0            ; Base
                db 0            ; Base
                db 0b10010010   ; P = 1, DPL = 0, S = 1, E = 0, D = 0, W = 1, A = 0
                db 0b11001111   ; G = 1, DB = 1, L = 0, Limit
                db 0
GDT_End:

GDT_Descriptor: ; Size of the descriptor should be subtracted by 1
                dw GDT_End - GDT_Start - 1      ; Size
                dw GDT_Start                    ; Offset

CODE_SEGMENT    equ code_descriptor - GDT_Start
DATA_SEGMENT    equ data_descriptor - GDT_Start
