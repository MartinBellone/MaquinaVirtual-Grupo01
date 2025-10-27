; Elimina de la lista circular doblemente enlazada los nodos con valor repetido y
; devuelve la cantidad de nodos eliminadaos

; PUSH <Doble puntero a nodo>
; CALL ELIMINA_REPETIDOS
; ADD SP, 4
; Devuelve en EAX la cantidad de nodos eliminados

NULL EQU -1
VAL EQU 0
ANT EQU 4
SIG EQU 8

ELIMINA_REPETIDOS:          PUSH BP
                            MOV BP, SP
                            SUB SP, 4
                            MOV [BP - 4], 0
                            PUSH EDX
                            PUSH EAX
                            PUSH EEX
                            PUSH ECX
                            PUSH EBX
                            PUSH EFX

                            MOV EDX, [BP + 8] ; EDX guarda el doble puntero a head
                            MOV EAX, [EDX] ; EAX apunta al primer nodo

 CICLO:                     MOV EBX, [EDX]
                            CMP EAX, [EBX + ANT] ; Si no pegue la vuelta sigo
                            JZ FIN
                            MOV EEX, [EDX] ; EEX apunta al anterior al primer nodo (Ultimo)
                            MOV EEX, [EEX + ANT]
CICLO2:                     CMP EEX, EAX
                            JZ FIN_CICLO2
                            CMP [EEX + VAL], [EAX + VAL]
                            JZ REPETIDO
                            MOV EEX, [EEX + ANT]
                            JMP CICLO2

REPETIDO:                   ADD [BP - 4], 1
                            MOV ECX, [EEX + SIG]
                            MOV [ECX + ANT], [EEX + ANT]
                            MOV EFX, [ECX + ANT]
                            MOV [EFX + SIG], ECX
                            MOV EEX, EFX
                            JMP CICLO2

FIN_CICLO2:                 MOV EAX, [EAX + SIG]
                            JMP CICLO

FIN:                        MOV EAX, [BP - 4]
                            POP ECX
                            POP EEX
                            POP EAX
                            POP EDX
                            MOV SP, BP
                            POP BP
                            RET



MAIN:                       PUSH BP
                            MOV SP, BP
                            