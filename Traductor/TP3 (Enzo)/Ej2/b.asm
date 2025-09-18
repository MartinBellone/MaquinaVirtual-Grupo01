; LEER [0]
; RESULTADO [4]

        MOV [4], 0
OTRO:   MOV ECX, DS
        LDL ECX, 1
        LDH ECX, 4
        MOV EAX, 0x01
        SYS 1


        CMP [0], 0 ; Si el valor es 0 sigo
        JZ SIGUE
        CMP [0], 1 ; Si el valor no es 0 ni 1, voy a FIN
        JNZ FIN

SIGUE:  SHL [4], 1 ; Desplazo a izquierda el resultado
        OR [4], [0]
        JMP OTRO

FIN:    MOV EDX, DS
        ADD EDX, 4
        LDL ECX, 4
        LDL ECX, 1
        MOV EAX, 0x01
        SYS 2
        STOP