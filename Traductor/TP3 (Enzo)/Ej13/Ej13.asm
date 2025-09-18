; Asumo que n y m vienen en [0] y [4]
; A partir de [8] tengo la matriz

            MOV EAX, 0 ; i
            MOV EBX, 0 ; j

CICLO:      CMP EAX, [0]
            JZ CUMPLE
            CMP EBX, EAX ; If(j < n) Con esto reviso el triangulo inferior
            JZ FIN_COLUMNA

            ; Formo x (x = i * m + j)
            MOV EDX, EAX
            MUL EDX, [4]
            ADD EDX, EBX

            ; Formo la direccion
            MUL EDX, 4
            ADD EDX, 8

            MOV ECX, [EDX]

            ; Formo x(x = j * m + i)
            MOV EDX, EBX
            MUL EDX, [4]
            ADD EDX, EAX

            ; Formo la direccion
            MUL EDX, 4
            ADD EDX, 8

            CMP [EDX], ECX
            JNZ NO_CUMPLE 

            ADD EBX, 1
            JMP CICLO


FIN_COLUMNA: ADD EAX, 1
            MOV EBX, 0
            JMP CICLO

CUMPLE:     MOV EFX, 1
            JMP FIN

NO_CUMPLE:  MOV EFX, 0


FIN         MOV EDX, [0]
            MUL EDX, [4]
            MUL EDX, 4
            ADD EDX, 8 ; EDX apunta a la primera posicion libre despues de la
            MOV [EDX], EFX
            LDL ECX, 1
            LDH ECX, 4
            MOV EAX, 0x01
            SYS 2

            STOP