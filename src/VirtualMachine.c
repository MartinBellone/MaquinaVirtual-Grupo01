#include "VirtualMachine.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "VM_memory.h"
#include "constants.h"
#include "instr_arith.h"
#include "instr_data.h"
#include "instr_logic.h"
#include "instr_sys.h"
#include "utils.h"

void menu(TVM *vm, int tipoOp1, int tipoOp2) {
    void (*func[])(TVM *vm, int tipoOp1, int tipoOp2) = {
        SYS, JMP, JZ, JP, JN, JNZ, JNP, JNN, NOT, invalidOpCode, invalidOpCode, invalidOpCode,
        invalidOpCode, invalidOpCode, invalidOpCode,
        STOP, MOV, ADD, SUB, MUL, DIV, CMP, SHL, SHR, SAR, AND, OR, XOR, SWAP, LDL, LDH,
        RND};
    func[vm->reg[OPC]](vm, tipoOp1, tipoOp2);
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
    unsigned char c, header[6], version;
    unsigned int codeSize;

    arch = fopen(fileName, "rb");
    if (arch == NULL)
        printf("ERROR al abrir el archivo \"%s\"\n", fileName);
    else {
        // Lectura del identificador y version del archivo
        fread(header, sizeof(char), 5, arch);
        header[5] = '\0';  // Asegurarse de que la cadena esté terminada en null

        if (strcmp(header, (unsigned char *)"VMX25") != 0) {
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
        unsigned char sizeBytes[2];
        if (fread(sizeBytes, sizeof(char), 2, arch) != 2) {
            fprintf(stderr, "ERROR: no se pudo leer tamaño de código\n");
            fclose(arch);
            exit(1);
        }

        codeSize = (sizeBytes[0] << 8) | sizeBytes[1];

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
}

void showCodeSegment(TVM *vm) {
    for (int i = 0; i < vm->tableSeg[0].size; i++) {
        printf("[%04x]: %X\n", i, vm->mem[i]);
    }
}

void initVm(TVM *vm) {
    vm->reg[CS] = 0;            // segmento de codigo
    vm->reg[IP] = vm->reg[CS];  // contador de instrucciones apunta al inicio del segmento de codigo
    vm->reg[DS] = 1 << 16;      // segmento de datos
}

void readOp(TVM *vm, int TOP, int numOp) {  // numOp es OP1 u OP2 y TOP tipo de operando

    if (TOP == 0b01) {
        vm->reg[numOp] = 0x01 << 24;             // registro
        vm->reg[numOp] |= vm->mem[vm->reg[IP]];  // lee el registro
        vm->reg[IP]++;                           // incrementa el contador de instrucciones
    } else {
        if (TOP == 0b10) {
            // inmediato: 2 bytes (16 bits). Encode as: TT ........ where TT=0x02 in the top byte
            unsigned int high = vm->mem[vm->reg[IP]];
            unsigned int low = vm->mem[vm->reg[IP] + 1];
            unsigned int imm = (high << 8) | low;
            imm = signExtend(imm, 2);
            // Store with type in top byte (0x02)
            vm->reg[numOp] = (0x02 << 24) | (imm & 0x00FFFFFF);
            vm->reg[IP] += 2;  // incrementa el contador de instrucciones
        } else {
            if (TOP == 0b11) {                                                                                                // memoria
                vm->reg[numOp] = 0x03 << 24;                                                                                  // memoria
                vm->reg[numOp] |= (vm->mem[vm->reg[IP]] << 16) | (vm->mem[vm->reg[IP] + 1] << 8) | vm->mem[vm->reg[IP] + 2];  // lee el operando de memoria
                vm->reg[IP] += 3;                                                                                             // incrementa el contador de instrucciones
            } else {
                printf("Error: Tipo de operando invalido.\n");
                exit(1);
            }
        }
    }
}

void readInstruction(TVM *vm) {
    unsigned char instruction;
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
    vm->reg[OP1] = 0;
    vm->reg[OP2] = 0;

    if (TOP2 != 0 && TOP1 != 0) {  // Hay dos operandos
        readOp(vm, TOP2, OP2);
        readOp(vm, TOP1, OP1);
    } else {
        if (TOP2 != 0) {  // Hay un operando
            readOp(vm, TOP2, OP1);
            TOP1 = TOP2;
            TOP2 = 0;
        }
    }

    menu(vm, TOP1, TOP2);
}

void executeProgram(TVM *vm) {
    printf("\n----- Ejecucion del programa -----\n");
    while (1) {
        readInstruction(vm);
        if (vm->reg[IP] >= vm->tableSeg[0].size) {
            exit(0);
        }
    }
}

void executeDisassembly(TVM *vm) {
    printf("----- Disassembler -----\n");
    int ip = vm->reg[IP];
    int maskOPC = 0b00011111;   // mascara para obtener el codigo de operacion
    int maskTOP1 = 0b00110000;  // mascara para obtener el primer operando
    int maskTOP2 = 0b11000000;  // mascara para obtener el segundo operando

    while (ip < vm->tableSeg[0].size) {
        unsigned char instruction;
        int operando1, operando2;
        int TOP1, TOP2, opc, i;

        instruction = vm->mem[ip];  // leo la instruccion
        printf("[%04X]  ", ip);
        printf("%02X ", instruction);

        // decodifica la instruccion
        TOP2 = (instruction & maskTOP2) >> 6;
        TOP1 = (instruction & maskTOP1) >> 4;
        opc = instruction & maskOPC;
        ip++;  // se para en el primer byte del segundo operando

        int total_bytes = TOP1 + TOP2;
        for (i = 0; i < total_bytes; i++) {
            printf(" %02X", vm->mem[ip + i] & 0xFF);
        }
        // Rellenar para alinear columnas (máximo 6 bytes extra)
        for (; i < 6; i++) printf("   ");
        printf("  |  %-8s ", MNEMONIC_NAMES[opc]);

        if (TOP2 == 1) {
            operando2 = vm->mem[ip];
            ip++;
        } else if (TOP2 == 2) {
            operando2 = (vm->mem[ip] << 8) | vm->mem[ip + 1];
            operando2 = signExtend(operando2, 2);
            ip += 2;
        } else if (TOP2 == 3) {
            operando2 = (vm->mem[ip] << 16) | (vm->mem[ip + 1] << 8) | vm->mem[ip + 2];
            ip += 3;
        } else
            operando2 = 0;

        if (TOP1 == 1) {
            operando1 = vm->mem[ip];
            ip++;
        } else if (TOP1 == 2) {
            operando1 = (vm->mem[ip] << 8) | vm->mem[ip + 1];
            ip += 2;
        } else if (TOP1 == 3) {
            operando1 = (vm->mem[ip] << 16) | (vm->mem[ip + 1] << 8) | vm->mem[ip + 2];
            ip += 3;
        } else
            operando1 = 0;

        // Operandos alineados
        int printed = 0;
        if (TOP1 == 3) {
            unsigned char codigoRegistro = (operando1 & 0xFF0000) >> 16;
            unsigned short int offset = operando1 & 0x00FFFF;
            printf("[%s + %u]", REGISTER_NAMES[codigoRegistro], offset);
            printed = 1;
        } else if (TOP1 == 2) {
            printf("%d", operando1);
            printed = 1;
        } else if (TOP1 == 1) {
            printf("%s", REGISTER_NAMES[operando1]);
            printed = 1;
        }
        if ((TOP1 != 0) && (TOP2 != 0)) {
            printf(", ");
            printed = 1;
        }
        if (TOP2 == 3) {
            unsigned char codigoRegistro = (operando2 & 0xFF0000) >> 16;
            unsigned short int offset = operando2 & 0x00FFFF;
            printf("[%s + %u]", REGISTER_NAMES[codigoRegistro], offset);
            printed = 1;
        } else if (TOP2 == 2) {
            printf("%d", operando2);
            printed = 1;
        } else if (TOP2 == 1) {
            printf("%s", REGISTER_NAMES[operando2]);
            printed = 1;
        }
        if (!printed) printf(" ");
        printf("\n");
    }
}