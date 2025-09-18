; La idea es ingresar los numeros e ir sumandolos, 
; luego ingreso la nueva lista y tambien sumo.
; La resta entre este total y el anterior es el 
; numero que falta en la nueva lista
;
; En [0] voy ingresando los numeros
; EBX acumulo suma de la lista 1
; Luego paso lo de EBX a [0]
; EFX guarda el n
; En EBX acumulo la suma de la lista 2
; Los numeros de la lista 2 los leo en [4]
; EEX es el contador 
; Luego resto [0] con EBX


        MOV EBX, 0
        MOV EFX, 0

; Leo numeros de la lista 1 y los acumulo en EBX
CICLO1: MOV EDX, DS
        LDL ECX, 1
        LDH ECX, 4
        MOV EAX, 0x01
        SYS 1
        CMP [EDX], 0
        JN CICLO2
        ADD EBX, [EDX]
        ADD EFX, 1
        JMP CICLO1

; Paso el resultado a [0]
        MOV [0], EBX
        MOV EBX, 0 ; Reinicio acumulador
        SUB EFX, 1 ; EFX guarda el n - 1

; Leo los numeros de la lista 2
        MOV EDX, DS
        ADD EDX, 4
        LDL ECX, 1
        LDH ECX, 4
        MOV EAX, 0x01

; Inicializo el contador de numeros
        MOV EEX, 0

CICLO2: SYS 1   
        CMP EEX, EFX
        JZ FIN
        ADD EBX, [EDX]
        JMP CICLO2

; Resto la suma de la lista 1 con la suma de la lista 2 y el 
; resultado lo guardo en [0]
FIN:    MOV EDX, DS
        SUB [0], EBX
        MOV [EDX], [0]
        LDL ECX, 1
        LDH ECX, 4
        MOV EAX, 0x01
        SYS 2
        STOP

        
        