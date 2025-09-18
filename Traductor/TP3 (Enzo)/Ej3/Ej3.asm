; LEER [0]
; resultado [4]
; EFX registro para ir guardando las potencias


            ; Leo la direccion
            MOV EFX, 1

            ; Leo el valor ingresado por teclado en la memoria en la posicion EDX (Inicio del Data Segment)
            MOV EDX, DS
            LDL ECX, 1
            LDH ECX, 4
            MOV EAX, 0x01
            SYS 1

            ; Si el numero es 0 ya paro, sino sigo dividiendo el numero
SIGUE:      CMP [EDX], 0
            JZ FIN
            DIV [EDX], 2
            CMP AC, 0 ; Si el resto es 0, aumento la potencia sino ya sumo la potencia de 2 al resultado
            JNZ MULTIPLICA
            ADD [4], EFX
            JMP SIGUE


MULTIPLICA: MUL EFX, 2
            JMP SIGUE

FIN:        MOV EDX, DS
            ADD EDX, 4
            LDL ECX, 4
            LDL ECX, 1
            MOV EAX, 0x10
            SYS 2
            STOP






; Version 2
    MOV EDX, DS
    LDL ECX, 1
    LDH ECX, 4
    MOV EAX, 0x01
    SYS 1

    MOV EDX, DS
    LDL ECX, 1
    LDH ECX, 4
    MOV EAX, 0x10
    SYS 2
    STOP