PUSH     BP
MOV      BP, SP
MOV      EAX, 1
CMP      l[BP+8], 0
JZ       41
MOV      EAX, l[BP+8]
SUB      EAX, 1
PUSH     EAX
CALL     0
ADD      SP, 4
MUL      EAX, l[BP+8]
MOV      SP, BP
POP      BP
RET
main: PUSH     BP
SYS 0xF
MOV      BP, SP
  MOV      EDX, l[BP+12]
  MOV      EDX, l[EDX]
  MOV      DH, l[EDX]
  SUB      DH, 48
  PUSH     DH
  CALL     0
  ADD      SP, 4
   MOV      EDX, DS
   MOV      l[EDX], EAX
  LDH      ECX, 4
  LDL      ECX, 1
 MOV      EAX, 9
SYS      2
 MOV      EDX, SP
 SUB      EDX, 24
   LDL      ECX, 6
   SYS      2
   ADD      BP, 12
   MOV      SP, BP
   POP      BP
   RET