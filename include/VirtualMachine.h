#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H

typedef struct {
    unsigned short int base;
    unsigned short int size;
} TSR;

typedef struct {
    char *vmxFile;
    char *vmiFile;
    int memSize;
    int disassembly;
    int argc;
    char *argv[];
} VMParams;

typedef struct {
    unsigned char *mem;  // char mem[16]
    int reg[32];         // 4 Bytes por cada entero, son 32 registros de 4 bytes cada uno
    TSR tableSeg[8];
    char *vmiFile;
    int argc;
    unsigned int argv;
} TVM;                   
                                                             // Type virtual machine
void initTSR(TVM *vm, unsigned short int sizes[7], unsigned short int cantSegments);  // Funcion para inicializar la tabla de segmentos
void readFileVMX(TVM *vm, char *fileName);                                            // Funcion para leer el archivo vmx
void initVm(TVM *vm, unsigned short int sizes[7], unsigned short int cantSegments);   // Funcion para inicializar la maquina virtual
void readInstruction(TVM *vm);
void readOp(TVM *vm, int TOP, int numOp);
void showCodeSegment(TVM *vm);
void executeProgram(TVM *vm);
void executeDisassembly(TVM *vm);
void parseArgs(int argc, char *argv[], VMParams *argsSalida, TVM *vm);
void buildParamSegment(TVM *vm, VMParams *params);
void readFileVMI(TVM *vm, char *fileName);
void writeFile(TVM *vm, char *fileName);

#endif  // VIRTUAL_MACHINE_H
