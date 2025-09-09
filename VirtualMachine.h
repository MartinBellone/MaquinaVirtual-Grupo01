typedef struct {
    unsigned short int base;
    unsigned short int size;
} TSR;

typedef struct {
    char mem[16384];  // char mem[16]
    int reg[32];      // 4 Bytes por cada entero, son 32 registros de 4 bytes cada uno
    TSR tableSeg[8];
} TVM;  // Type virtual machine

int convertToPhysicalAddress(TVM *vm);
void readMemory(TVM *vm);
void writeMemory(TVM *vm);
void readFile(TVM *vm, char *fileName);  // Funcion para leer el archivo vmx
void initVm(TVM *vm);                    // Funcion para inicializar la maquina virtual
void readInstruction(TVM *vm);
void readOp(TVM *vm, int TOP, int numOp);

int getOp(TVM *vm, int registerValue);
void setOp(TVM *vm, int registerValue, int value);

void MOV(TVM *vm, int tipoOp1, int tipoOp2);
