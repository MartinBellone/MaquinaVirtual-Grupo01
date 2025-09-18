; LEER [0]
; RESULTADO [4]
; EFX va a guardar el contador


            ; Leo el numero que esta en la posicion 0 de memoria
            MOV EFX, 2
            MOV EDX, DS
            LDL ECX, 1
            LDH ECX, 4
            MOV EAX, 0x01
            SYS 1
            MOV EBX, [EDX]

            ; Si es 2 cuenta como primo
            CMP [EDX], 2
            JZ FIN

OTRO:       CMP EBX, EFX ; Resto el numero con el contador
            JNP PRIMO ; Si hice n - n es porque ya no hay numeros para probar
            DIV EBX, EFX
            CMP AC, 0 ; Si el resto de hacer la division da 0 (Se guarda en AC) encontre un divisor, termino
            JZ NO_PRIMO
            ADD EFX, 1
            JMP OTRO

PRIMO:      MOV EBX, 1
            JMP FIN
    
NO_PRIMO:   MOV EBX, 0

FIN:        MOV EDX, DS
            ADD EDX, 4
            MOV [EDX], EBX
            LDL ECX, 1
            LDH ECX, 4
            MOV EAX, 0X01
            SYS 2
            STOP