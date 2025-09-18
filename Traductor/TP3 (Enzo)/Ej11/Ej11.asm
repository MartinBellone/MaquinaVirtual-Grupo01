; [0] guarda el n
; A partir de [4] se guardan los n numeros
; EFX es el aux para formar la direccion
; EBX es el contador del ciclo
; EEX guarda el n


; Leo el n
            MOV EDX, DS
            LDL ECX, 1
            LDH ECX, 4
            MOV EAX, 0x01
            SYS 1

            MOV EEX, [EDX]
            MOV EBX, 1

; Leo valores del vector
INICIO:     CMP EBX, EEX
            JZ SIGUE
            MOV EFX, EBX
            MUL EFX, 4
            ADD EFX, DS
            MOV EDX, EFX
            LDL ECX, 1
            LDH ECX, 4
            MOV EAX, 0x01
            SYS 1
            ADD EBX, 1
            JMP INICIO

            MOV EBX, 1

; En la celda EEX (n) * 4 termina el vector
; Guarda el maximo en un registro ECX

            MOV ECX, -99999

; Busco el maximo
SIGUE:      CMP EBX, EEX
            JZ CONTAR
            MOV EFX, EBX
            MUL EFX, 4
            ADD EFX, DS
            CMP [EFX], ECX
            JNN NUEVO_MAX
            ADD EBX, 1
            JMP SIGUE

NUEVO_MAX:  MOV ECX, [EFX]
            ADD EBX, 1
            JMP SIGUE


; ECX guarda el maximo
; EAX guarda la cantidad de veces que aparece el numero maximo

            MOV EAX, 0
            MOV EBX, 1
CONTAR:     CMP EBX, EEX
            JZ FIN
            MOV EFX, EBX
            MUL EFX, 4
            ADD EFX, DS
            CMP [EFX], ECX
            JZ SUMAR
            ADD EBX, 1
            JMP CONTAR


SUMAR:      ADD EAX, 1
            ADD EBX, 1
            JMP CONTAR


FIN:        MOV EDX, EEX
            MUL EDX, 4
            ADD EDX, 4; n + 4 por el maximo
            MOV [EDX], ECX
            LDL ECX, 1
            LDH ECX, 4
            MOV EAX, 0x01
            SYS 2

            ADD EDX, 4; n + 4 + 4 por la cantidad
            MOV [EDX], EAX
            LDL ECX, 1
            LDH ECX, 4
            MOV EAX, 0x01
            SYS 2
            STOP