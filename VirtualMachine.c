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

void SYS(TVM *vm, int tipoOp1, int tipoOp2) {
    // TODO
}

void JMP(TVM *vm, int tipoOp1, int tipoOp2) {
    // TODO
}

void JZ(TVM *vm, int tipoOp1, int tipoOp2) {
    // TODO
}

void JP(TVM *vm, int tipoOp1, int tipoOp2) {
    // TODO
}

void JN(TVM *vm, int tipoOp1, int tipoOp2) {
    // TODO
}

void JNZ(TVM *vm, int tipoOp1, int tipoOp2) {
    // TODO
}

void JNP(TVM *vm, int tipoOp1, int tipoOp2) {
    // TODO
}

void JNN(TVM *vm, int tipoOp1, int tipoOp2) {
    // TODO
}

void NOT(TVM *vm, int tipoOp1, int tipoOp2) {
    // TODO
}

void STOP(TVM *vm, int tipoOp1, int tipoOp2) {
    // TODO
}

void CMP(TVM *vm, int tipoOp1, int tipoOp2) {
    // TODO
}

void SHL(TVM *vm, int tipoOp1, int tipoOp2) {
    // TODO
}

void SHR(TVM *vm, int tipoOp1, int tipoOp2) {
    // TODO
}

void SAR(TVM *vm, int tipoOp1, int tipoOp2) {
    // TODO
}

void AND(TVM *vm, int tipoOp1, int tipoOp2) {
    // TODO
}

void OR(TVM *vm, int tipoOp1, int tipoOp2) {
    // TODO
}

void XOR(TVM *vm, int tipoOp1, int tipoOp2) {
    // TODO
}

void SWAP(TVM *vm, int tipoOp1, int tipoOp2) {
    // TODO
}

void LDL(TVM *vm, int tipoOp1, int tipoOp2) {
    // TODO
}

void LDH(TVM *vm, int tipoOp1, int tipoOp2) {
    // TODO
}

void RND(TVM *vm, int tipoOp1, int tipoOp2) {
    // TODO
}

void ADD(TVM *vm, int tipoOp1, int tipoOp2) {
    // TODO
}

void SUB(TVM *vm, int tipoOp1, int tipoOp2) {
    // TODO
}

void MUL(TVM *vm, int tipoOp1, int tipoOp2) {
    // TODO
}

void DIV(TVM *vm, int tipoOp1, int tipoOp2) {
    // TODO
}

void MOV(TVM *vm, int tipoOp1, int tipoOp2) {
    // int mask=0x00FFFFFF;
    // int opAux;
    // opAux = vm->reg[OP2] & mask; //obtengo el operando sin el tipo
    // opAux = (opAux << 8) >> 8; //extiendo el signo
    // vm->reg[vm->reg[OP1] & mask] = opAux; //muevo el valor al registro destino

    if (tipoOp1 == 1) {      // registro
        if (tipoOp2 == 1) {  // registro
            // MOV EDX,EEX
            // 01010000 0E 0D
            // OP1 = 0x0100000D OP2 = 0x0100000E
            vm->reg[vm->reg[OP1] & 0x00FFFFFF] = vm->reg[vm->reg[OP2] & 0x00FFFFFF];
        } else if (tipoOp2 == 2) {  // inmediato
            // 0x0200000A
            // 0x00000A00
            // 0x0000000A
            //  el shift a la derecha es aritmético, por lo que extiende el signo
            vm->reg[vm->reg[OP1] & 0x00FFFFFF] = (vm->reg[OP2] << 8) >> 8;  // extiendo el signo
        } else if (tipoOp2 == 3) {                                          // memoria
            // MOV EAX, [EDX+4]
            // MOV EAX, [EDX]
            // MOV EAX, [4] = MOV EAX, [DS+4]
            //  TODO corregir
            unsigned short int registro = (vm->reg[OP2] & 0x1F0000) >> 19;  // obtengo el registro

            vm->reg[LAR] = (vm->reg[DS] & 0xFFFF0000) | (vm->reg[OP2] & 0x0000FFFF);  // cargo LAR con segmento de datos y offset del operando
            readMemory(vm);
            vm->reg[vm->reg[OP1] & 0x00FFFFFF] = vm->reg[MBR];
        }
    } else if (tipoOp1 == 2) {                                                        // directo
        if (tipoOp2 == 0) {                                                           // registro
            vm->reg[LAR] = (vm->reg[DS] & 0xFFFF0000) | (vm->reg[OP1] & 0x0000FFFF);  // cargo LAR con segmento de datos y offset del operando
            vm->reg[MBR] = vm->reg[vm->reg[OP2] & 0x00FFFFFF];
            writeMemory(vm);
        } else if (tipoOp2 == 1) {                                                    // inmediato
            vm->reg[LAR] = (vm->reg[DS] & 0xFFFF0000) | (vm->reg[OP1] & 0x0000FFFF);  // cargo LAR con segmento de datos y offset del operando
            vm->reg[MBR] = (vm->reg[OP2] << 8) >> 8;                                  // extiendo el signo
            writeMemory(vm);
        }
    }
}

void invalidOpCode(TVM *vm, int tipoOp1, int tipoOp2) {
    printf("Error: Invalid Mnemonic Code");
    exit(1);
}
void menu(TVM *vm, int tipoOp1, int tipoOp2) {
    void (*func[])(TVM *vm, int tipoOp1, int tipoOp2) = {
        SYS, JMP, JZ, JP, JP, JN, JNZ, JNP, JNN, NOT, invalidOpCode, invalidOpCode,
        invalidOpCode, invalidOpCode, invalidOpCode, invalidOpCode,
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
    printf("Segment: %d\n", segment);
    baseSeg = vm->tableSeg[segment].base;
    offSeg = vm->reg[LAR] & 0x0000FFFF;
    printf("Base: 0x%X Offset: 0x%X\n", baseSeg, offSeg);
    return 0x00000000 | (baseSeg + offSeg);
}
int signExtend(unsigned int value, int nbytes) {
    int shift = (4 - nbytes) * 8;           // cuántos bits correr
    return (int)(value << shift) >> shift;  // extiende signo al castear
}

void readMemory(TVM *vm) {
    int physAddr = convertToPhysicalAddress(vm);
    // tiene que venir el MAR seteado con la cantidad de bytes a leer
    vm->reg[MAR] |= physAddr;
    int bytesToRead = (vm->reg[MAR] & 0xFFFF0000) >> 16;
    int address = (vm->reg[MAR] & 0x0000FFFF);
    vm->reg[MBR] = 0x00000000;  // inicializo MBR en 0
    int acc = 0;
    for (int i = 1; i <= bytesToRead; i++) {
        unsigned char b = vm->mem[address + i];
        acc = (acc << 8) | b;  // MSB primero
        // debug opcional:
        // printf("Reading @0x%X = 0x%X\n", address + i, b);
    }
    // vm->reg[MBR] = vm->mem[address];
    // debo tener en cuenta el signo
    vm->reg[MBR] = signExtend(acc, bytesToRead);
}

void writeMemory(TVM *vm) {
    int physAddr = convertToPhysicalAddress(vm);
    // tiene que venir el MAR seteado con la cantidad de bytes a escribir y el MBR con los datos a escribir
    vm->reg[MAR] |= physAddr;
    printf("Physical Address to write: 0x%X\n", vm->reg[MAR]);
    int bytesToWrite = (vm->reg[MAR] & 0xFFFF0000) >> 16;
    int address = (vm->reg[MAR] & 0x0000FFFF);
    for (int i = 0; i < bytesToWrite; i++) {
        // 0x0004000A
        printf("Writing to memory address 0x%X: 0x%X\n", vm->reg[MAR] + i, vm->reg[MBR] >> (8 * (bytesToWrite - i - 1)) & 0xFF);
        vm->mem[address + i] = vm->reg[MBR] >> (8 * (bytesToWrite - i - 1)) & 0xFF;  // escribo byte a byte
    }
}
void createLogicAdress(TVM *vm) {
    // int segment, offset;
    // segment = vm->reg[LAR] & 0xFFFF0000; //obtengo el segmento
    // offset = vm->reg[LAR] & 0x0000FFFF; //obtengo el offset
    // vm->reg[LAR] = (vm->reg[DS] & 0xFFFF0000) | offset; //cargo LAR con segmento de datos y offset del operando
}
void initTSR(TVM *vm, unsigned short int size) {
    vm->tableSeg[0].base = 0;
    vm->tableSeg[0].size = size;
    vm->tableSeg[1].base = size;
    int cantBytes = size;
    vm->tableSeg[1].size = 16384 - cantBytes;
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

void initVm(TVM *vm) {
    vm->reg[CS] = 0;            // segmento de codigo
    vm->reg[IP] = vm->reg[CS];  // contador de instrucciones apunta al inicio del segmento de codigo
    vm->reg[DS] = 1 << 16;      // segmento de datos
}

void readOp(TVM *vm, int TOP, int numOp) {  // numOp es OP1 u OP2 y TOP tipo de operando

    if (TOP == 0b01) {                              // registro
        vm->reg[numOp] = vm->mem[vm->reg[IP] + 1];  // lee el registro
        vm->reg[IP]++;                              // incrementa el contador de instrucciones
    } else {
        if (TOP == 0b10) {
            vm->reg[numOp] = (vm->mem[vm->reg[IP] + 1] << 8) | vm->mem[vm->reg[IP] + 2];  // lee el inmediato
            vm->reg[IP] += 2;                                                             // incrementa el contador de instrucciones
        } else {
            // TODO parte de memoria
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
    readOp(vm, OP2, TOP2);
    readOp(vm, OP1, TOP1);

    menu(vm, TOP1, TOP2);
}
