; Devuelve promedio de numeros naturales, frena cuando se ingrega un negativo
; CALL PROMEDIO_NATURALES
; Devuelve en EAX el promedio

PROMEDIO_NATURALES:         PUSH BP
                            MOV BP, SP
                            PUSH EDX
                            PUSH ECX
                            SUB SP, 4 ; Variable local para leer
                            SUB SP, 4 ; Variable local para el resultado
                            SUB SP, 4 ; Variable local para el contador

                            MOV [BP-8], 0
                            MOV [BP-12], 0

                            MOV EDX, BP
                            SUB EDX, 4
                            LDL ECX, 1
                            LDH ECX, 4
                            MOV EAX, 1
                            SYS 1
CICLO:                      CMP [BP-4], 0
                            JN DIVIDIR
                            ADD [BP-12], 1
                            ADD [BP-8], [BP-4]
                            SYS 1
                            JMP CICLO

DIVIDIR:                    CMP [BP-12], 0
                            JZ FIN
                            DIV [BP-8], [BP-12]

FIN:                        MOV EAX, [BP-8]
                            POP ECX
                            POP EDX
                            MOV SP, BP
                            POP BP
                            RET


MAIN:                       PUSH BP
                            MOV BP, SP
                            SUB SP, 4 ; Variable local al main para el resultado
                            SYS 0xF
                            CALL PROMEDIO_NATURALES
                            MOV [BP-4], EAX
                            MOV EDX, BP
                            SUB EDX, 4
                            LDL ECX, 1
                            LDH ECX, 4
                            MOV EAX, 1
                            SYS 2
                            RET