MOV [DS], 3
MOV [DS+4], 3
MOV [DS+8], 1
MOV [DS+12], 1
MOV [DS+16], 1
MOV [DS+20], 1
MOV [DS+24], 1
MOV [DS+28], 1
MOV [DS+32], 1
MOV [DS+36], 1
MOV [DS+40], 1
MOV EAX, 0
MOV ECX, DS
ADD ECX, 8
cicloi: CMP EAX, [0]
JNN simetrica
MOV EBX,0
cicloj: CMP EBX, [4]
JNN avanzoi
MOV EDX, EBX
MUL EDX, [4]
ADD EDX, EAX
MUL EDX, 4
ADD EDX, 8
ADD EDX, DS
CMP [EDX], [ECX]
JNZ nosimetrica
ADD EBX, 1
ADD ECX, 4
JMP cicloj
avanzoi: ADD EAX, 1
JMP cicloi
simetrica: MOV EEX, 1
JMP fin
nosimetrica: MOV EEX, 0
fin: MOV EAX, 1
MOV EDX, DS
ADD EDX, 100
MOV [EDX], EEX
LDH ECX, 4
LDL ECX, 1
SYS 2
STOP

