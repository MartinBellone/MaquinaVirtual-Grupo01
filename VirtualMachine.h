typedef struct {
    unsigned short int base;
    unsigned short int size;
} TSR;

typedef struct{
    char *mem; // char mem[16]
    int reg[32]; // 4 Bytes por cada entero, son 32 registros de 4 bytes cada uno
    TSR tableSeg[8];

} TVM; // Type virtual machine

void readMemory(/*Completar*/);
void writeMemory(/*Completar*/);
void readFile(TVM * vm); // Funcion para leer el archivo vmx
void readIntruction(TVM * vm);