; LEER [0]
; RESULTADO [4]
; CONTADOR EFX nesecito contar las potencias

; La idea es que si el numero que se quiere ingresar es 1010

; Primero me viene el 0
; Como el valor es 0 aumento la potencia (EFX = 2)


; Ahora viene el 1
; [4] tiene 0000, le sumo 2 y me queda 0010


; Ahora viene el 0
; Como el valor es 0 aumento la potencia (EFX = 4)

; Ahora viene el 1
; [4] tiene 0010, le sumo 4 y me queda 1010

; Asi logro armar el numero binario por medio de sumas

        MOV [4], 0
        MOV EFX, 1
OTRO:   MOV ECX, DS
        LDL ECX, 1
        LDH ECX, 4
        MOV EAX, 0x01
        SYS 1


        CMP [0], 0 ; Si el valor es 0 sigo
        JZ MULTIPLICA ; Si el valor es 0, aumento la potencia
        CMP [0], 1 ; Si el valor no es 0 ni 1, voy a FIN
        JNZ FIN

        ; Si el valor no es 0 pero es 1
        ADD [4], EFX
        
MULTIPLICA: MUL EFX, 2
        JMP OTRO

FIN:    MOV EDX, DS
        ADD EDX, 4
        LDL ECX, 4
        LDL ECX, 1
        MOV EAX, 0x01
        SYS 2
        STOP