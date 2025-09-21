;Utilizando el algoritmo anterior, construya un programa que calcule el factorial de un número

        ; --- Lectura: leer un entero decimal y guardarlo en [DS+4] ---
inicio: MOV     EAX, 0x01        ; modo lectura decimal
        MOV     EDX, DS
        ADD     EDX, 4           ; offset donde guardar input
        LDH     ECX, 0x04        ; tamaño de celda = 4 bytes (alto de ECX)
        LDL     ECX, 0x01        ; cantidad = 1 (bajo de ECX)
        SYS     0x1              ; READ

        ; Cargar n en EBX (no lo tocaremos)
        MOV     EBX, [EDX]       ; EBX = n

        ; Si n == 0 o n == 1 -> resultado = 1
        CMP     EBX, 0
        JZ      print_one
        CMP     EBX, 1
        JZ      print_one

        ; Inicializaciones:
        MOV     EFX, 1           ; EFX = resultado (empieza en 1)
        MOV     EEX, 2           ; EEX = i (comienza en 2)

loop_outer: CMP     EBX, EEX         ; compara n con i  (EBX - EEX)
        JNN     do_multiply      ; si n >= i (>=0) -> multiplicar
        JMP     print_result     ; si n < i -> termine


        ; queremos: EFX = EFX * EEX
        ; implementamos multiplicación por sumas repetidas:
do_multiply: MOV     EAX, 0           ; EAX será producto temporal = 0
        MOV     ECX, EEX         ; ECX = contador = i

inner_loop: CMP     ECX, 0
        JZ      after_inner
        ADD     EAX, EFX         ; producto += EFX
        SUB     ECX, 1
        JMP     inner_loop

after_inner: MOV     EFX, EAX         ; EFX = producto calculado
        ADD     EEX, 1           ; i++
        JMP     loop_outer

print_one: MOV     EFX, 1
        ; caer a print_result
  ; Escribir resultado en memoria y llamar a SYS 2 para mostrarlo
print_result: MOV     EDX, DS
        ADD     EDX, 8           ; offset donde guardamos el resultado (ejemplo)
        MOV     [EDX], EFX       ; guardar 4 bytes del resultado
        MOV     EAX, 0x01        ; modo escritura decimal
        LDH     ECX, 0x04        ; tamaño 4 bytes
        LDL     ECX, 0x01        ; cantidad 1
        SYS     0x2              ; WRITE
        STOP