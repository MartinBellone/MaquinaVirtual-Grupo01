; EAX guarda n
; EEX guarda n - 1
; EBX guarda el contador
; ECX guarda el resultado

INICIO:
        MOV EDX,DS
        MOV EAX,1
        LDL ECX,1
        LDH ECX,4
        SYS 1

        MOV EAX, [EDX] ; Obtengo n

        CMP EAX, 1
        JZ FIN
        CMP EAX, 0
        JZ FIN

        MOV EEX, EAX
        SUB EEX, 1 ; Obtengo n - 1
        MOV EBX, EEX ; Inicializo contador
        MOV ECX, 0

MULT_LOOP:
        CMP EBX, 0
        JZ LOOP
        ADD ECX, EAX
        SUB EBX, 1
        JMP MULT_LOOP

LOOP:   MOV EAX, ECX ; Nuevo n
        SUB EEX, 1   ; Nuevo n - 1
        MOV EBX, EEX ; Actualizo contador
        CMP EEX, 1
        JZ FIN
        JMP MULT_LOOP

FIN:     STOP