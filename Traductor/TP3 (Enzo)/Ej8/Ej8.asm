; Leo en la posicion 0 de memoria
; Guardo el resultado en la posicion 4 de memoria
; Uso EAX para guardar el numero
; Uso EBX para contar los bits en 1
; Uso ECX para obtener el ultimo bit

INICIO:         MOV EDX, DS
                LDL ECX, 1
                LDH ECX, 4
                MOV EAX, 0x01
                SYS 1

                MOV EAX, [EDX]


                CMP EAX, 0
                JN FIN
                XOR EBX, EBX

LOOP:           CMP EAX, 0
                JZ MOSTRAR
                MOV ECX, EAX
                AND ECX, 0x01
                SHR EAX, 1
                CMP ECX, 0
                JZ LOOP
                ADD EBX, 1
                JMP LOOP

MOSTRAR:        MOV EDX, DS
                ADD EDX, 4
                MOV [EDX], EBX
                LDL ECX, 1
                LDH ECX, 4
                MOV EAX, 0x01
                SYS 2
                JMP INICIO

FIN:            STOP