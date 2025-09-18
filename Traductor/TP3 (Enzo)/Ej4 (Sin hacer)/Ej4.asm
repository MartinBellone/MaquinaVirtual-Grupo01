# EBX contiene el valor de A
# ECX contiene el valor de B
# EAX tendra el resultado de A * B
# EFX es un contador

; Supongo que viene A = 2 y B = 4

                    MOV EAX, 0
                    CMP EBX, 0
                    MOV EDX, EAX ; Guardo signo
                    SHR EDX 15 
                    MOV EEX, EBX ; Guardo signo
                    SHR EEX, 15
                    CMP EDX, EEX ; Si tienen mismo signo seran positivos sino negativos
                    JZ  MISMO_SIGNO
                    JMP DIFERENTE_SIGNO
MISMO_SIGNO:        MOV EFX, 0   ; Es positivo
                    JMP CAMBIAR_SIGNO_A


DIFERENTE_SIGNO:    MOV EFX, 1  ; Es negativo


CAMBIAR_SIGNO_A     NOT EBX
                    ADD EBX, 1

CAMBIAR_SIGNO_B     NOT ECX
                    ADD ECX, 1
                    
                    
                    MOV EDX, 0 ; Lo uso de contador

BUCLE:              CMP EDX, EBX
                    JZ  RESULTADO
                    ADD EAX, ECX
                    ADD EDX, 1
                    JMP BUCLE


RESULTADO:          CMP EFX, 0
                    JNZ RESULTADO_NEGATIVO
                    JMP FIN

RESULTADO_NEGATIVO: NOT EAX
                    ADD EAX, 1
FIN:                STOP