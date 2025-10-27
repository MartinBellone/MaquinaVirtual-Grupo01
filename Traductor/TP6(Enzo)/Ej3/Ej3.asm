\\INCLUDE "heap.asm"
SIZE_INT EQU 4

; Muestra un entero
; PUSH <entero>
; CALL PRINT_INT
; ADD SP, 4
PRINT_INT:  PUSH BP
            MOV BP, SP
            PUSH EAX
            PUSH EDX
            PUSH ECX

            MOV EDX, BP
            ADD EDX, 8 ; EDX = &x

            LDL ECX, 1
            LDH ECX, 4
            MOV EAX, 1
            SYS 2

            POP ECX
            POP EDX
            POP EAX
            MOV SP, BP
            POP BP
            RET

; Sum devuelve en EAX la suma
SUM:    PUSH BP
        MOV BP, SP
        PUSH EAX
        PUSH ECX
        PUSH EDX
        PUSH EEX
        PUSH EFX

        MOV EDX, [BP + 12] ; EDX = &z
        MOV EEX, [BP + 8] ; EEX = &v[0]

        MOV EFX, [DS]
        MOV [DS], 0 ; s = 0 es una variable estatica, funciona como global pero solo dentro de la funcion (No va al Stack, va al DS)
        MOV ECX, 0 ; ECX = i register indica que se debe guardar en un registro

CICLO:  CMP ECX, [EDX]
        JZ FIN
        MOV EAX, ECX ; EAX = i
        MUL EAX, 4 ; EAX = i * 4
        ADD EAX, EEX ; EAX = &v[i]
        ADD [DS], [EAX] ; s += v[i]
        ADD ECX, 1 ; i += 1
        JMP CICLO

FIN:    MOV EAX, [DS] ; EAX = s

        MOV [DS], EFX
        POP EEX
        POP EDX
        POP ECX
        POP EAX
        MOV SP, BP
        POP BP
        RET

FUNCION1:   PUSH BP
            MOV BP, SP
            PUSH EAX
            PUSH EDX
            PUSH ECX
            PUSH EDX

            MOV EAX, [BP + 8] ; EAX = *z (*z es el valor de la variable local del main, pero BP + 8 es la variable local z de esta funcion)
            MOV ECX, [BP + 12] ; ECX = &v[0]

            SUB SP, 4 ; Reservo espacio para s
            SUB SP, 4 ; Reservo espacio para i

            MOV [ECX], 1 ; v[0] = 1

            MOV [BP - 4], 1 ; i = 1

CICLO:      CMP [BP - 4], EAX ; i < *z
            JZ FIN
            MOV EDX, [BP - 4] ; EDX = i
            MUL EDX, 4 ; EDX = i * 4
            ADD EDX, ECX ; EDX = &v[i]
            MOV [EDX], [EDX - 4] ; v[i] = v[i - 1]
            MUL [EDX], [BP - 4] ; v[i] = v[i] * i
            ADD [BP - 4], 1 ; i += 1
            JMP CICLO

FIN:        MOV EDX, BP
            ADD EDX, 8 ; EDX = &z (Pero este z es el parametro, no la variable local del main)
            PUSH EDX ; Paso &z como parametro
            PUSH ECX ; Paso &v[0] como parametro
            CALL SUM
            ADD SP, 8

            MOV [BP - 8], EAX ; s = SUM(&v[0], z)

            PUSH [BP - 8] ; Paso s como parametro
            CALL PRINT_INT
            ADD SP, 4

            ADD SP, 8
            POP EDX
            POP ECX
            POP EDX
            POP EAX
            MOV SP, BP
            POP BP
            RET


MAIN:   PUSH BP
        MOV BP, SP
        PUSH EAX
        PUSH EDX
        PUSH EEX
        PUSH ECX

        CALL heap_init

        PUSH SIZE_INT
        CALL alloc
        ADD SP, 4

        MOV EDX, EAX ; Guarda la direccion reservada

        MOV ECX, SIZE_INT
        MUL ECX, 100 ; ECX = sizeof(int) * 100

        PUSH ECX
        CALL alloc
        ADD SP, 4

        MOV EEX, EAX ; EEX = &v[0]

        MOV [EDX], 100 ; *z = 100

        PUSH [EDX] ; Paso *z como parametro
        PUSH EEX ; Paso &v[0] como parametro
        CALL FUNCION1
        ADD SP, 8

        POP ECX
        POP EEX
        POP EDX
        POP EAX
        POP BP
        MOV SP, BP
        RET

