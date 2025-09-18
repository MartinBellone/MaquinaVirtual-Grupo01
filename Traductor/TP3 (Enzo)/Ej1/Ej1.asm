; LEER [0] Elijo esta direccion de memoria para ir leyendo los numeros
; RESULTADO [4] Elijo esta direccion de memoria para guardar el resultado
; CONTADOR EFX uso un registro como auxiliar para contar los numeros
; Cada celda de memoria ocupa 1 byte, como los registros son de 4 bytes, 
; asumo que los datos tambien son de 4 bytes y me ocuparan 4 celdas cada uno

        MOV [4], 0 ; Inicializo el resultado en 0
        MOV EFX, 0 ; Inicializo el contador en 0

        ; Preparo la lectura por teclado
 OTRO:  MOV EDX, DS ; EDX guarda la direccion 0 de la memoria, donde se almacenan los numeros que ingreso
        LDL ECX, 1 ; LDL carga el valor 1 en los 2 bytes menos significativos de ECX
        LDH ECX, 4 ; LDH carga el valor 4 en los 2 bytes mas significativos de ECX
        MOV EAX, 0x01 ; El registro EAX guarda la base que se lee (0x01 dice que se lee en base decimal)
        SYS 1 ; Llama al sistema para leer el número

        ; Inicio del while (Condicion)
        CMP [EDX], 0
        JN SIGUE

        ; Como el numero es positivo, estoy dentro del while
        ADD EFX, 1 ; Incremento el contador
        ADD [4], [EDX] ; Acumulo el resultado
        JMP OTRO ; Vuelvo para leer otro numero
SIGUE:  CMP EFX, 0 ; Verifico si el contador es 0
        JZ FIN

        ; Contador no es 0
        DIV [4], EFX

FIN:    MOV EDX, DS
        ADD EDX, 4 ; EDX apunta a la posicion 4 de memoria, donde esta almacenado el resultado
        LDL ECX, 4
        LDL ECX, 1
        MOV EAX, 0x01
        SYS 2
        STOP