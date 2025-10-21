STR:  PUSH BP
      MOV BP, SP
      PUSH EAX ; Para almacenar mis parametros
      PUSH ECX ; Para almacenar mis parametros
      PUSH EDX ; Para almacenar mis parametros
      PUSH AC ; Para almacenar mis parametros ya que el resto de la division pisa el valor de AC
      MOV EAX, [BP+12] ; Cargo el entero
      MOV EBX, [BP+8] ; Cargo la direccion donde debo escribir el string
      MOV ECX, EAX ; Copio el entero en ECX
      DIV ECX, 10 ; 
      CMP ECX, 0 ; Si el cociente es 0, ya no debo llamar mas a la funcion
      JZ fin ; Si es 0, salto a fin
      MOV EDX, AC ;
      PUSH ECX ; Apilo el cociente
      PUSH EBX ; Apilo la direccion ya que yo quiero que el primer numero que se escriba sea el mas a la izquierda
      CALL STR ; Llamo recursivamente
      ADD SP, 8 ; Limpio la pila
      MOV AC, EDX ; Cargo el resto en AC
fin:  ADD AC, '0' ; Convierto el resto a su valor ASCII
      MOV b[EBX], AC ; Escribo el caracter en la posicion de memoria
      ADD EBX, 1 ; Le sumo uno a la posicion de memoria para escribir '\0'
      MOV b[EBX], 0 ; Escribo el caracter nulo que si hay otro digito, sera pisado
      POP EDX ; Recupero EDX
      POP AC ; Recupero AC
      POP ECX ; Recupero ECX
      POP EAX ; Recupero EAX
      MOV SP, BP
      POP BP 
      RET 

main: MOV EAX, 1
MOV EDX, DS
LDL ECX, 1
LDH ECX, 4
SYS 1

MOV EBX, DS
ADD EBX, 100
MOV ECX, EBX

PUSH [EDX]; puntero a donde va el string 
PUSH EBX ; numero a convertirlo en string
CALL STR
ADD SP, 8
MOV EBX, ECX
; en EBX esta el puntero al numero pasado a string 

MOV EDX, EBX
SYS 4
RET
