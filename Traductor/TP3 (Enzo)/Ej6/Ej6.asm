; EAX contiene resultado de la division
; EEX guarda el signo para el resultado
; EBX guarda el primer operando
; ECX guarda el segundo operando
; AC guarda el resto

INICIO:         MOV EDX, DS
                LDL ECX, 1
                LDH ECX, 4
                MOV EAX, 0x01
                SYS 1

                MOV EBX, [EDX]

                MOV EDX, DS
                ADD EDX, 4
                LDL ECX, 1
                LDH ECX, 4
                MOV EAX, 0x01
                SYS 1

                MOV ECX, [EDX]

                CMP ECX, 0
                JZ FIN

                MOV EAX, 0
                MOV EEX, 0
                MOV AC, 0


                CMP EBX, 0
                JNN POSITIVO
                MOV EEX, 1
                NOT EBX
                ADD EBX, 1

POSITIVO:       CMP ECX, 0
                JNN DIV_LOOP
                MOV EEX, 1
                NOT ECX
                ADD ECX, 1

DIV_LOOP:       CMP EBX, ECX
                JN  APLICAR_SIGNO
                ADD EAX, 1
                SUB EBX, ECX
                JMP DIV_LOOP

APLICAR_SIGNO:  MOV AC, EBX
                NOT AC
                ADD AC, 1
                CMP EEX, 1
                JNZ FIN
                NOT EAX
                ADD EAX, 1

FIN:            STOP
