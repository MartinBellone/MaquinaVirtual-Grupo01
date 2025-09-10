#include "VirtualMachine.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LAR 0
#define MAR 1
#define MBR 2
#define IP 3  // registro del contador de instrucciones
#define OPC 4
#define OP1 5
#define OP2 6
#define EAX 10  // A
#define EBX 11  // B
#define ECX 12  // C
#define EDX 13  // D
#define EEX 14  // E
#define EFP 15  // F
#define AC 16   // 1F
#define CC 17   // 2F
#define CS 26
#define DS 27

#define CC_N 0x80000000
#define CC_Z 0x40000000

void setCC(TVM *vm,int value){
    if (value<0)
        vm->reg[CC] = CC_N;
    else if (value == 0)
        vm->reg[CC] = CC_Z;
}

void SYS(TVM *vm, int tipoOp1, int tipoOp2) {
    int call = vm->reg[OP1];
    int tamanioCelda = (vm->reg[ECX] & 0xFFFF0000) >> 16;
    int cantLecturas = vm->reg[ECX] & 0x0000FFFF;
    if(vm->reg[EAX] < 0 || vm->reg[EAX] > 10 || vm->reg[EAX] % 2 != 0)
        exit(1);

    vm->reg[LAR] = vm->reg[EDX]; // Cargo LAR con la direcicon logica
    vm->reg[MAR] = tamanioCelda << 16; // Cargo MAR con la cantidad de bytes a leer

    for(int i = 0; i < cantLecturas; i++)
        if(call == 1){
            printf("[%08x]: ", vm->reg[EDX]);
            if(vm->reg[EAX] == 0){
                char valor;
                scanf(" %c", &valor);
                vm->reg[MBR] = valor;
                writeMemory(vm);
            } 
            else 
                if(vm->reg[EAX] == 1){
                    char valor;
                    scanf(" %c", &valor);
                    vm->reg[MBR] = valor;
                    writeMemory(vm);
                }
                else
                    if(vm->reg[EAX] == 2 | vm->reg[EAX] == 4 | vm->reg[EAX] == 8){
                        unsigned short int valor;
                        scanf("%hu", &valor);
                        vm->reg[MBR] = valor;
                        writeMemory(vm);
                    }
                    else
                        // No es una base valida
                        exit(1);
        }
        else 
            if(call == 2)
                if(vm->reg[EAX] == 0){
                    int valor;
                    readMemory(vm);
                    valor = vm->reg[MBR];
                    printf("%d", valor);
                }  
                else 
                    if(vm->reg[EAX] == 1){
                        char valor;
                        readMemory(vm);
                        valor = vm->reg[MBR];
                        printf("%c", valor);
                    }
                    else
                        if(vm->reg[EAX] == 2 | vm->reg[EAX] == 4 | vm->reg[EAX] == 8){
                            unsigned short int valor;
                            readMemory(vm);
                            valor = vm->reg[MBR];
                            printf("%hu", valor);
                        }
                        else
                            // No es una base valida
                            exit(1);
}

void JMP(TVM *vm, int tipoOp1, int tipoOp2) {
    vm->reg[IP] = getOp(vm, vm->reg[OP1]);
}

void JZ(TVM *vm, int tipoOp1, int tipoOp2) {
    if (vm->reg[CC] == CC_Z) {
        vm->reg[IP] = getOp(vm, vm->reg[OP1]);
    }
}

void JP(TVM *vm, int tipoOp1, int tipoOp2) {
    if (vm->reg[CC] != CC_N && vm->reg[CC] != CC_Z) {
        vm->reg[IP] = getOp(vm, vm->reg[OP1]);
    }
}

void JN(TVM *vm, int tipoOp1, int tipoOp2) {
    if (vm->reg[CC] == CC_N) {
        vm->reg[IP] = getOp(vm, vm->reg[OP1]);
    }
}

void JNZ(TVM *vm, int tipoOp1, int tipoOp2) {
    if (vm->reg[CC] != CC_Z) {
        vm->reg[IP] = getOp(vm, vm->reg[OP1]);
    }
}

void JNP(TVM *vm, int tipoOp1, int tipoOp2) {
    if (vm->reg[CC] == CC_N || vm->reg[CC] == CC_Z) {
        vm->reg[IP] = getOp(vm, vm->reg[OP1]);
    }
}

void JNN(TVM *vm, int tipoOp1, int tipoOp2) {
    if (vm->reg[CC] != CC_N) {
        vm->reg[IP] = getOp(vm, vm->reg[OP1]);
    }
}

void NOT(TVM *vm, int tipoOp1, int tipoOp2) {
    int value = getOp(vm, vm->reg[OP1]);
    value = ~value;
    setOp(vm, vm->reg[OP1], value);
    setCC(vm, value);
}

void STOP(TVM *vm, int tipoOp1, int tipoOp2) {
    exit(0);
}

void CMP(TVM *vm, int tipoOp1, int tipoOp2) {
    int value1, value2, result;
    value1 = getOp(vm, vm->reg[OP1]);
    value2 = getOp(vm, vm->reg[OP2]);
    result = value1 - value2;
    setCC(vm, result);
}

void SHL(TVM *vm, int tipoOp1, int tipoOp2) {
    int value1,value2;
    value1 = getOp(vm,vm->reg[OP1]);
    value2 = getOp(vm,vm->reg[OP2]);
    value1 = value1 << value2;
    setCC(vm,value1);
    setOp(vm,vm->reg[OP1],value1);
}

void SHR(TVM *vm, int tipoOp1, int tipoOp2) {
    int value1, value2, mascara = 0xFFFFFFFF;
    value1 = getOp(vm,vm->reg[OP1]);
    value2 = getOp(vm,vm->reg[OP2]);
    value1 &= ~(mascara << (32 - value2)); 
    /*
    
        Ej:
    
        value1 = 1111;
        value2 = 1;

        value1 >> value2
        En C quedaria 1111 (Mantiene signo) tendria que ser 0111
        
        Entonces nuestra mascara que es 1111 hago
        1111 << (4 - 1) = 1000
        ~1000 = 0111

        Value1 quedaria:
        1111 & 0111 = 0111

    */
    setCC(vm,value1);
    setOp(vm,vm->reg[OP1],value1);
}

void SAR(TVM *vm, int tipoOp1, int tipoOp2) {
    int value1,value2;
    value1 = getOp(vm,vm->reg[OP1]);
    value2 = getOp(vm,vm->reg[OP2]);
    value1 = value1 >> value2;
    setCC(vm,value1);
    setOp(vm,vm->reg[OP1],value1);
}

void AND(TVM *vm, int tipoOp1, int tipoOp2) {
    int value1,value2;
    value1 = getOp(vm,vm->reg[OP1]);
    value2 = getOp(vm,vm->reg[OP2]);
    value1 = value1 & value2;
    setCC(vm,value1);
    setOp(vm,vm->reg[OP1],value1);
}

void OR(TVM *vm, int tipoOp1, int tipoOp2) {
    int value1,value2;
    value1 = getOp(vm,vm->reg[OP1]);
    value2 = getOp(vm,vm->reg[OP2]);
    value1 = value1 | value2;
    setCC(vm,value1);
    setOp(vm,vm->reg[OP1],value1);
}

void XOR(TVM *vm, int tipoOp1, int tipoOp2) {
    int value1,value2;
    value1 = getOp(vm,vm->reg[OP1]);
    value2 = getOp(vm,vm->reg[OP2]);
    value1 = value1 ^ value2;
    setCC(vm,value1);
    setOp(vm,vm->reg[OP1],value1);
}

void SWAP(TVM *vm, int tipoOp1, int tipoOp2) {
    int aux = getOp(vm, OP2);
    setOp(vm, OP2, OP1);
    setOp(vm, OP1, aux);
}

void LDL(TVM *vm, int tipoOp1, int tipoOp2) {
    int value1,value2,aux,mask = 0xFFFF0000;
    value1 = getOp(vm,vm->reg[OP1]);
    value2 = getOp(vm,vm->reg[OP2]);
    value1 &= mask; 
    value1 |= value2;
    setOp(vm,vm->reg[OP1],value1);
}

void LDH(TVM *vm, int tipoOp1, int tipoOp2) {
    int value1,value2,aux,mask = 0x0000FFFF;
    value1 = getOp(vm,vm->reg[OP1]);
    value2 = getOp(vm,vm->reg[OP2]);
    value1 &= mask; 
    value1 |= value2 << 16;
    setOp(vm,vm->reg[OP1],value1);
}

void RND(TVM *vm, int tipoOp1, int tipoOp2) {
    int value1,value2;
    value1 = getOp(vm,vm->reg[OP1]);
    value2 = getOp(vm,vm->reg[OP2]);
    //TODO terminar
}

void ADD(TVM *vm, int tipoOp1, int tipoOp2) {
    int value1,value2;
    value1 = getOp(vm,vm->reg[OP1]);
    value2 = getOp(vm,vm->reg[OP2]);
    value1 += value2;
    setCC(vm,value1);
    setOp(vm,vm->reg[OP1],value1);
}

void SUB(TVM *vm, int tipoOp1, int tipoOp2) {
    int value1,value2;
    value1 = getOp(vm,vm->reg[OP1]);
    value2 = getOp(vm,vm->reg[OP2]);
    value1 -= value2;
    setCC(vm,value1);
    setOp(vm,vm->reg[OP1],value1);
}

void MUL(TVM *vm, int tipoOp1, int tipoOp2) {
    int value1,value2;
    value1 = getOp(vm,vm->reg[OP1]);
    value2 = getOp(vm,vm->reg[OP2]);
    value1 *= value2;
    setCC(vm,value1);
    setOp(vm,vm->reg[OP1],value1);
}

void DIV(TVM *vm, int tipoOp1, int tipoOp2) {
    int value1,value2;
    value1 = getOp(vm,vm->reg[OP1]);
    value2 = getOp(vm,vm->reg[OP2]);
    vm->reg[AC] = value1 % value2;
    value1 /= value2; //hace division entera porque ambos son enteros
    setCC(vm,value1);
    setOp(vm,vm->reg[OP1],value1);
}

void MOV(TVM *vm, int tipoOp1, int tipoOp2) {
    int value1,value2;
    value1 = getOp(vm,vm->reg[OP1]);
    value2 = getOp(vm,vm->reg[OP2]);
    setOp(vm,value1,value2);
}

void invalidOpCode(TVM *vm, int tipoOp1, int tipoOp2) {
    printf("Error: Invalid Mnemonic Code");
    exit(1);
}
void menu(TVM *vm, int tipoOp1, int tipoOp2) {
    void (*func[])(TVM *vm, int tipoOp1, int tipoOp2) = {
        SYS, JMP, JZ, JP, JP, JN, JNZ, JNP, JNN, NOT, invalidOpCode, invalidOpCode,
        invalidOpCode, invalidOpCode, invalidOpCode,
        STOP, MOV, ADD, SUB, MUL, DIV, CMP, SHL, SHR, SAR, AND, OR, XOR, SWAP, LDL, LDH,
        RND};
    func[vm->reg[OPC]](vm, tipoOp1, tipoOp2);
}

int getOp(TVM *vm, int registerValue) {
    int type = (registerValue & 0xFF000000) >> 24;  // obtengo el tipo de operando
    int opAux = registerValue & 0x00FFFFFF;         // obtengo el operando sin el tipo

    if (type == 0b01) {  // registro
        return vm->reg[opAux];
    } else if (type == 0b10) {                      // inmediato
        return (opAux << 8) >> 8;                   // extiendo el signo
    } else if (type == 0b11) {                      // memoria
        int registro = (opAux & 0x1F0000) >> 19;    // obtengo el registro
        int offset = opAux & 0x0000FFFF;            // obtengo el offset
        vm->reg[LAR] = vm->reg[registro] + offset;  // cargo LAR con segmento de datos y offset del operando
        vm->reg[MAR] = 0x00040000;                  // seteamos MAR para leer 4 bytes
        readMemory(vm);
        return vm->reg[MBR];
    }
}
void setOp(TVM *vm, int registerValue, int value) {
    int mask = 0x00FFFFFF;
    int type = (registerValue & 0xFF000000) >> 24;  // obtengo el tipo de operando
    int opAux = registerValue & mask;               // obtengo el operando sin el tipo
    if (type == 0b01) {                             // registro
        vm->reg[opAux] = value;
    } else if (type == 0b10) {  // inmediato
        printf("Error: No se puede escribir en un inmediato.\n");
        exit(1);
    } else if (type == 0b11) {                    // memoria
        int registro = (opAux & 0x1F0000) >> 19;  // obtengo el registro
        int offset = opAux & 0x0000FFFF;          // obtengo el offset
        // cargo LAR con el contenido del registro (debera ser un puntero) y offset del operando
        vm->reg[LAR] = vm->reg[registro] + offset;
        vm->reg[MBR] = value;
        vm->reg[MAR] = 0x00040000;
        writeMemory(vm);
    }
}

int convertToPhysicalAddress(TVM *vm) {
    int segment, baseSeg, offSeg;
    segment = (vm->reg[LAR] & 0xFFFF0000) >> 16;  // obtengo el segmento

    if (segment > 7) {  // si el segmento es mayor a 7, error
        printf("Error: Segmentation fault.\n");
        exit(1);
    }

    baseSeg = vm->tableSeg[segment].base;
    offSeg = vm->reg[LAR] & 0x0000FFFF;
    return 0x00000000 | (baseSeg + offSeg);
}

void readMemory(TVM *vm) {
    int physAddr = convertToPhysicalAddress(vm);
    // tiene que venir el MAR seteado con la cantidad de bytes a leer
    vm->reg[MAR] |= physAddr;
    int bytesToRead = (vm->reg[MAR] & 0xFFFF0000) >> 16;
    vm->reg[MBR] = 0;  // inicializo MBR en 0
    for (int i = 1; i <= bytesToRead; i++) {
        vm->reg[MBR] |= (vm->mem[vm->reg[MAR] + i - 1] << (8 * (bytesToRead - i)));  // leo byte a byte
    }
    vm->reg[MBR] = vm->mem[vm->reg[MAR]];
}

void writeMemory(TVM *vm) {
    int physAddr = convertToPhysicalAddress(vm);
    // tiene que venir el MAR seteado con la cantidad de bytes a escribir y el MBR con los datos a escribir
    vm->reg[MAR] |= physAddr;
    int bytesToWrite = (vm->reg[MAR] & 0xFFFF0000) >> 16;
    for (int i = 1; i <= bytesToWrite; i++) {
        // 0x0004000A
        vm->mem[vm->reg[MAR] + i - 1] = vm->reg[MBR] >> (8 * (bytesToWrite - i)) & 0xFF;  // escribo byte a byte
    }
}
void createLogicAdress(TVM *vm) {
    // int segment, offset;
    // segment = vm->reg[LAR] & 0xFFFF0000; //obtengo el segmento
    // offset = vm->reg[LAR] & 0x0000FFFF; //obtengo el offset
    // vm->reg[LAR] = (vm->reg[DS] & 0xFFFF0000) | offset; //cargo LAR con segmento de datos y offset del operando
}
void initTSR(TVM *vm, unsigned short int size) {
    vm->tableSeg[0].base=0;
    vm->tableSeg[0].size=size;
    vm->tableSeg[1].base=size;
    vm->tableSeg[1].size = 16384 - size;
}

void readFile(TVM *vm, char *fileName) {
    // funcion para leer el vmx
    FILE *arch;  // TODO arreglar con writeMemory
    int i = 0;   // direccion de memoria a guardar el byte
    char c, header[6], version, sizeBytes[2];
    unsigned short int codeSize;

    arch = fopen(fileName, "rb");
    if (arch == NULL)
        printf("ERROR al abrir el archivo");
    else {
        // Lectura del identificador y version del archivo
        fread(header, sizeof(char), 5, arch);
        header[5] = '\0';  // Asegurarse de que la cadena esté terminada en null
        printf("Identificador del archivo: %s\n", header);

        if (strcmp(header, "VMX25") != 0) {
            fprintf(stderr, "ERROR: formato de archivo incorrecto (header %s)\n", header);
            fclose(arch);
            exit(1);
        }
        fread(&version, sizeof(char), 1, arch);
        if (version != 1) {
            fprintf(stderr, "ERROR: versión no soportada (%d)\n", version);
            fclose(arch);
            exit(1);
        }
        printf("Version de la maquina virtual archivo: %d\n", (int)version);
        char sizeBytes[2];
        if (fread(sizeBytes, sizeof(char), 2, arch) != 2) {
            fprintf(stderr, "ERROR: no se pudo leer tamaño de código\n");
            fclose(arch);
            exit(1);
        }

        codeSize = (sizeBytes[0] << 8) | sizeBytes[1];

        printf("sizeBytes[0]: %d sizeBytes[1]: %d\n", sizeBytes[0], sizeBytes[1]);
        printf("Tamanio del segmento de codigo: %u bytes\n", codeSize);

        initTSR(vm, codeSize);

        if (vm->tableSeg[1].size <= 0) {  // si se supera el tamaño del segmento de codigo, salir
            printf("Error: El programa es demasiado grande para la memoria asignada.\n");
        } else {
            // Leer codigo
            while (fread(&c, sizeof(char), 1, arch) == 1) {
                vm->mem[i] = c;
                i++;
            }
            initVm(vm);
        }

        fclose(arch);
    }
    // TODO cambiar los prints y revisar en general
}

void showCodeSegment(TVM *vm){
    for(int i = 0; i < vm->tableSeg[0].size; i++){
        printf("[%04x]: %02x\n", i, vm->mem[i]);
    }
}

void initVm(TVM *vm) {
    vm->reg[CS] = 0;            // segmento de codigo
    vm->reg[IP] = vm->reg[CS];  // contador de instrucciones apunta al inicio del segmento de codigo
    vm->reg[DS] = 1 << 16;      // segmento de datos
}

void readOp(TVM *vm, int TOP, int numOp) {  // numOp es OP1 u OP2 y TOP tipo de operando

    if (TOP == 0b01) { 
        vm->reg[numOp] = 0x01 << 24;                 // registro
        vm->reg[numOp] |= vm->mem[vm->reg[IP]];     // lee el registro
        vm->reg[IP]++;                              // incrementa el contador de instrucciones
    } else {
        if (TOP == 0b10) {
            vm->reg[numOp] = 0x02 << 24;                // inmediato
            vm->reg[numOp] |= (vm->mem[vm->reg[IP]] << 8) | vm->mem[vm->reg[IP] + 1];      // lee el inmediato
            vm->reg[IP] += 2;                                                             // incrementa el contador de instrucciones
        } else {
            if(TOP == 0b11){ // memoria
                vm->reg[numOp] = 0x03 << 24; // memoria
                vm->reg[numOp] |= (vm->mem[vm->reg[IP]] << 16) | (vm->mem[vm->reg[IP] + 1] << 8) | vm->mem[vm->reg[IP] + 2]; // lee el operando de memoria
                vm->reg[IP] += 3; // incrementa el contador de instrucciones
            }
            else{
                printf("Error: Tipo de operando invalido.\n");
                exit(1);
            }
        }
    }
}

void readInstruction(TVM *vm) {
    char instruction;
    int maskOPC = 0b00011111;   // mascara para obtener el codigo de operacion
    int maskTOP1 = 0b00110000;  // mascara para obtener el primer operando
    int maskTOP2 = 0b11000000;  // mascara para obtener el segundo operando
    int TOP1, TOP2;

    instruction = vm->mem[vm->reg[IP]];  // leo la instruccion
    // decodifica la instruccion
    TOP2 = (instruction & maskTOP2) >> 6;
    TOP1 = (instruction & maskTOP1) >> 4;
    vm->reg[OPC] = instruction & maskOPC;
    vm->reg[IP]++;  // se para en el primer byte del segundo operando
    // lee los operandos
    readOp(vm, TOP2, OP2);
    readOp(vm, TOP1, OP1);
    menu(vm, TOP1, TOP2);
}


void executeProgram(TVM *vm) {
    while (1) {
        readInstruction(vm);
    }
}
