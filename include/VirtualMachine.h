#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H

typedef struct {
    unsigned short int base;
    unsigned short int size;
} TSR;

typedef struct {
    char mem[16384];  // char mem[16]
    int reg[32];      // 4 Bytes por cada entero, son 32 registros de 4 bytes cada uno
    TSR tableSeg[8];
    int dFlag;
} TVM;  // Type virtual machine

void readFile(TVM *vm, char *fileName);  // Funcion para leer el archivo vmx
void initVm(TVM *vm);                    // Funcion para inicializar la maquina virtual
void readInstruction(TVM *vm);
void readOp(TVM *vm, int TOP, int numOp);
void showCodeSegment(TVM *vm);
void executeProgram(TVM *vm);
void executeDisassembly(TVM *vm);

void MOV(TVM *vm, int tipoOp1, int tipoOp2);

#endif  // VIRTUAL_MACHINE_H
