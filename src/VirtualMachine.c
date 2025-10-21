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

void initTSR(TVM *vm, unsigned short int sizes[7], unsigned short int cantSegments) {
    int j = 0;  // Indice de escritura en la tabla de segmentos
    for (int i = 0; i < cantSegments; i++) {
        if (sizes[i] != 0) {
            vm->tableSeg[j].size = sizes[i];
            if (j == 0)
                vm->tableSeg[j].base = 0;
            else
                vm->tableSeg[j].base = vm->tableSeg[j - 1].base + vm->tableSeg[j - 1].size;
            j++;
        }
    }
}

void parseArgs(int argc, char *argv[], VMParams *argsSalida, TVM *vm) {
    argsSalida->vmxFile = NULL;
    argsSalida->vmiFile = NULL;
    argsSalida->memSize = 16 * 1024;  // default 16KiB
    argsSalida->disassembly = 0;
    argsSalida->argc = 0;

    for (int i = 1; i < argc; i++) {
        if (strstr(argv[i], ".vmx")) {
            argsSalida->vmxFile = argv[i];
        } else if (strstr(argv[i], ".vmi")) {
            argsSalida->vmiFile = argv[i];
        } else if (strncmp(argv[i], "m=", 2) == 0) {
            argsSalida->memSize = atoi(argv[i] + 2) * 1024;  // m=64 → 64KiB
        } else if (strcmp(argv[i], "-d") == 0) {
            argsSalida->disassembly = 1;
        } else if (strcmp(argv[i], "-p") == 0) {
            // todo lo que sigue son parámetros del programa
            // este argv no se pasa a la VM, es un array que luego sera el param segment
            for (int j = i + 1; j < argc; j++) {
                strcpy(argsSalida->argv[argsSalida->argc++], argv[j]);
            }
            break;
        } else {
            printf("Argumento desconocido: %s\n", argv[i]);
        }
    }
    // mostrar param segment
    for (int i = 0; i < argsSalida->argc; i++) {
        printf("argv[%d]: %s\n", i, argsSalida->argv[i]);
    }

    if (!argsSalida->vmxFile) {
        fprintf(stderr, "ERROR: No se especificó archivo .vmx\n");
        exit(1);
    }
    if (argsSalida->vmiFile){
        vm->vmiFile = argsSalida->vmiFile;
    }
}
void buildParamSegment(TVM *vm, VMParams *argsSalida) {
    unsigned int base = 0;  // El segmento de parámetros siempre inicia en 0
    unsigned int offset = 0;
    unsigned int ptrs[argsSalida->argc];  // direcciones de cada string

    if (argsSalida->argc == 0) {
        vm->reg[PS] = -1;
        return;
    }

    // Copiar los strings uno detrás del otro
    for (int i = 0; i < argsSalida->argc; i++) {
        ptrs[i] = offset;                     // dirección del inicio del string i
        int len = strlen(argsSalida->argv[i]) + 1;  // incluye el '\0'
        memcpy(&vm->mem[offset], argsSalida->argv[i], len);
        offset += len;
    }

    // Agregar marcador de fin de punteros (-1)
    ptrs[argsSalida->argc] = 0xFFFFFFFF;

    // Copiar los punteros al final de la sección de strings
    vm->argv = offset;  // dirección del inicio del arreglo de punteros
    for (int i = 0; i <= argsSalida->argc; i++) {
        memcpy(&vm->mem[offset], &ptrs[i], sizeof(int));
        offset += sizeof(int);
    }

    // Configurar el segmento en la tabla
    vm->tableSeg[0].base = base;
    vm->tableSeg[0].size = offset;

    // Registrar el segmento PS
    vm->reg[PS] = base;
}

void readFileVMX(TVM *vm, char *fileName) {
    // funcion para leer el vmx
    FILE *arch;
    unsigned char c, header[6], version;
    unsigned int CSsize;
    unsigned short int sizes[7] = {0};  // tamanos de los segmentos
    arch = fopen(fileName, "rb");
    if (arch == NULL)
        printf("ERROR al abrir el archivo \"%s\"\n", fileName);
    else {
        // Lectura del identificador y version del archivo
        fread(header, sizeof(char), 5, arch);
        header[5] = '\0';  // Asegurarse de que la cadena esté terminada en null

        if (strcmp(header, (unsigned char *)"VMX25") != 0) {
            printf("ERROR: formato de archivo incorrecto (header %s)\n", header);
            exit(1);
        }
        fread(&version, sizeof(char), 1, arch);
        unsigned char CSsizeBytes[2];
        if (fread(CSsizeBytes, sizeof(char), 2, arch) != 2) {
            printf("ERROR: no se pudo leer tamaño de código\n");
            exit(1);
        }
        CSsize = (CSsizeBytes[0] << 8) | CSsizeBytes[1];
        if (version == 0x01) {
            sizes[0] = CSsize;  // CS
            sizes[1] = 16384 - CSsize;
            sizes[2] = 0;      // el entrypoint es el inicio del CS
            vm->reg[PS] = -1;  // no hay segmento de parametros
            vm->reg[KS] = -1;  // no hay segmento de constantes
            vm->reg[ES] = -1;  // no hay segmento de datos
            vm->reg[SS] = -1;  // no hay segmento de stack
            //             sizes[0] = vm->tableSeg[0].size;  // PS
            // sizes[1] = KSsize;                // KS
            // sizes[2] = CSsize;                // CS
            // sizes[3] = DSsize;                // DS
            // sizes[4] = ESsize;                // ES
            // sizes[5] = SSsize;                // SS
            // sizes[6] = entryPoint;            // entry point
            initTSR(vm, sizes, 2);
            initVm(vm, sizes, 2);
        } else if (version == 0x02) {
            unsigned char DSsizeBytes[2];
            if (fread(DSsizeBytes, sizeof(char), 2, arch) != 2) {
                printf("ERROR: no se pudo leer tamaño de segmento de datos\n");
                exit(1);
            }
            unsigned int DSsize = (DSsizeBytes[0] << 8) | DSsizeBytes[1];

            unsigned char ESsizeBytes[2];
            if (fread(ESsizeBytes, sizeof(char), 2, arch) != 2) {
                printf("ERROR: no se pudo leer tamaño de segmento extra\n");
                exit(1);
            }
            unsigned int ESsize = (ESsizeBytes[0] << 8) | ESsizeBytes[1];
            unsigned char SSsizeBytes[2];
            if (fread(SSsizeBytes, sizeof(char), 2, arch) != 2) {
                printf("ERROR: no se pudo leer tamaño de segmento de stack\n");
                exit(1);
            }
            unsigned int SSsize = (SSsizeBytes[0] << 8) | SSsizeBytes[1];

            unsigned char KSsizeBytes[2];
            if (fread(KSsizeBytes, sizeof(char), 2, arch) != 2) {
                fprintf(stderr, "ERROR: no se pudo leer tamaño de segmento de constantes \n");
                fclose(arch);
                exit(1);
            }
            unsigned int KSsize = (KSsizeBytes[0] << 8) | KSsizeBytes[1];

            if (CSsize + DSsize + ESsize + SSsize + KSsize > 16384) {
                printf("Error: El programa es demasiado grande para la memoria asignada.\n");
                fclose(arch);
                exit(1);
            }
            unsigned char entryPointBytes[2];
            if (fread(entryPointBytes, sizeof(char), 2, arch) != 2) {
                printf("ERROR: no se pudo leer el entry point\n");
                exit(1);
            }
            unsigned int entryPoint = (entryPointBytes[0] << 8) | entryPointBytes[1];

            sizes[0] = vm->tableSeg[0].size;  // PS
            sizes[1] = KSsize;                // KS
            sizes[2] = CSsize;                // CS
            sizes[3] = DSsize;                // DS
            sizes[4] = ESsize;                // ES
            sizes[5] = SSsize;                // SS
            sizes[6] = entryPoint;            // entry point
            initTSR(vm, sizes, 6);
            sizes[0] = CSsize;  // CS
            sizes[1] = DSsize;  // DS
            sizes[2] = ESsize;  // ES
            sizes[3] = SSsize;  // SS
            sizes[4] = KSsize;  // KS
            initVm(vm, sizes, 5);
        } else {
            printf("ERROR: versión de archivo incorrecta (%d)\n", version);
            exit(1);
        }

        // Leer codigo
        int i = 0;  // direccion de memoria a guardar el byte
        unsigned int codeSegment = vm->reg[CS] >> 16;
        i = vm->tableSeg[codeSegment].base;
        int cantLecturas = 0;
        while (fread(&c, sizeof(char), 1, arch) == 1 && cantLecturas < CSsize) {
            vm->mem[i] = c;
            i++;
            cantLecturas++;
        }
        unsigned int KSsegment = vm->reg[KS] >> 16;  // el registro KS existe aunque la version sea 1
        if (version == 2 && vm->tableSeg[KSsegment].size > 0) {
            // debo inicializar el segmento de constantes
            i = vm->tableSeg[KSsegment].base;
            while (fread(&c, sizeof(char), 1, arch) == 1) {
                vm->mem[i] = c;
                i++;
            }
        }
        fclose(arch);
    }
}

void readFileVMI(TVM *vm, char *fileName) {
    FILE *arch;
    unsigned char header[6], version;
    unsigned int dato;  // 4 bytes por registro
    unsigned short int base, size, tamanioMem;

    if (arch == NULL)
        printf("ERROR al abrir el archivo %s : ", fileName);
    else {
        fread(header, sizeof(char), 5, arch);
        header[5] = '\0';  // Asegurarse de que la cadena esté terminada en null

        if (strcmp(header, (unsigned char *)"VMI25") != 0) {
            printf("ERROR: formato de archivo incorrecto (header %s)\n", header);
            exit(1);
        }

        fread(&version, sizeof(char), 1, arch);

        if (version != 0x01) {
            printf("ERROR: versión de archivo incorrecta (%d)\n", version);
            exit(1);
        }

        fread(&tamanioMem, sizeof(tamanioMem), 1, arch);

        // Lectura de registros
        int i;
        for (i = 0; i < 32; i++) {
            if (fread(&dato, sizeof(int), 1, arch) != 1) {
                printf("ERROR: no se pudo leer el registro %d\n", i);
                exit(1);
            }
            vm->reg[i] = dato;
        }

        // Lectura de tabla de segmentos
        tamanioMem = 0;
        for (i = 0; i < 8; i++) {
            if (fread(&base, sizeof(base), 1, arch) != 1) {
                printf("ERROR: no se pudo leer el segmento %d\n", i);
                exit(1);
            }
            if (fread(&size, sizeof(size), 1, arch) != 1) {
                printf("ERROR: no se pudo leer el tamaño del segmento %d\n", i);
                exit(1);
            }
            vm->tableSeg[i].base = base;
            vm->tableSeg[i].size = size;
            tamanioMem += size;
        }

        vm->mem = malloc(tamanioMem * sizeof(unsigned char));

        // Lectura de memoria
        for (i = 0; i < tamanioMem; i++) {
            if (fread(&dato, sizeof(unsigned char), 1, arch) != 1) {
                printf("ERROR: no se pudo leer la memoria en la posición %d\n", i);
                exit(1);
            }
            vm->mem[i] = (unsigned char)dato;
        }
        fclose(arch);
    }
}

void writeFile(TVM *vm, char *fileName) {
    FILE *arch;
    unsigned char header[6] = "VMI25";
    unsigned char version = 0x01;
    if ((arch = fopen(fileName, "wb")) == NULL)
        printf("ERROR al crear el archivo %s : ", fileName);
    else {
        // Escritura del identificador y version del archivo
        fwrite(header, sizeof(char), 5, arch);
        fwrite(&version, sizeof(char), 1, arch);

        // Escritura de registros
        for (int i = 0; i < 32; i++) {
            fwrite(&vm->reg[i], sizeof(int), 1, arch);
        }

        int tamanioMem = 0;
        // Escritura de tabla de segmentos
        for (int i = 0; i < 8; i++) {
            fwrite(&vm->tableSeg[i].base, sizeof(unsigned short int), 1, arch);
            fwrite(&vm->tableSeg[i].size, sizeof(unsigned short int), 1, arch);
            tamanioMem += vm->tableSeg[i].size;
        }

        // Escritura de memoria
        for (int i = 0; i < tamanioMem; i++) {
            fwrite(&vm->mem[i], sizeof(unsigned char), 1, arch);
        }

        fclose(arch);
    }
}

void showCodeSegment(TVM *vm) {
    for (int i = 0; i < vm->tableSeg[0].size; i++) {
        printf("[%04x]: %X\n", i, vm->mem[i]);
    }
}

void initVm(TVM *vm, unsigned short int sizes[7], unsigned short int cantSegments) {
    unsigned short int totalSize = 0;
    int value;
    for (int i = 0; i < cantSegments; i++) {
        if (sizes[i] != 0)
            vm->reg[26 + i] = i << 16;
        else
            vm->reg[26 + i] = -1;  // segmento no usado
        totalSize += sizes[i];
    }
    vm->mem = (unsigned char *)malloc(totalSize * sizeof(unsigned char));
    if (vm->reg[SS] != -1) {
        vm->reg[SP] = vm->reg[SS] + vm->tableSeg[(vm->reg[SS] >> 16)].size;  // inicializo el stack pointer en el tope del segmento de stack
        // inicializo el instruction pointer en la parte alta con con la direccion del CS y en la parte baja con el entry point
        vm->reg[IP] = vm->reg[CS] | sizes[7];

        // Inicializo Stack
        vm->reg[SP] -= 4;
        value = vm->argv;
        vm->reg[LAR] = vm->reg[SP];  // cargo el registro LAR con el stack segment
        unsigned int physAddr = convertToPhysicalAddress(vm);
        vm->mem[physAddr] = (value >> 24) & 0xFF;      // byte más significativo
        vm->mem[physAddr + 1] = (value >> 16) & 0xFF;  // segundo byte
        vm->mem[physAddr + 2] = (value >> 8) & 0xFF;   // tercer byte
        vm->mem[physAddr + 3] = value & 0xFF;          // byte menos significativo

        vm->reg[SP] -= 4;
        value = vm->argc;
        vm->reg[LAR] = vm->reg[SP];  // cargo el registro LAR con el segmento de parametros
        physAddr = convertToPhysicalAddress(vm);
        vm->mem[physAddr] = (value >> 24) & 0xFF;      // byte más significativo
        vm->mem[physAddr + 1] = (value >> 16) & 0xFF;  // segundo byte
        vm->mem[physAddr + 2] = (value >> 8) & 0xFF;   // tercer byte
        vm->mem[physAddr + 3] = value & 0xFF;          // byte menos significativo

        vm->reg[SP] -= 4;
        vm->reg[LAR] = vm->reg[SP];  // cargo el registro LAR con el segmento de stack
        physAddr = convertToPhysicalAddress(vm);
        vm->mem[physAddr] = 0XFF;
        vm->mem[physAddr + 1] = 0XFF;
        vm->mem[physAddr + 2] = 0XFF;
        vm->mem[physAddr + 3] = 0XFF;
    }
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
        if (vm->reg[IP] >= vm->tableSeg[0].size) {
            exit(0);
        } else {
            if (vm->reg[IP] == -1)  // instruccion STOP
                exit(0);
        }
        readInstruction(vm);
    }
}

void executeDisassembly(TVM *vm) {
    printf("----- Disassembler -----\n");

    int ip = vm->reg[IP], segmento = ip >> 16, i;
    ip = ip & 0x0000FFFF;
    int maskOPC = 0b00011111;   // mascara para obtener el codigo de operacion
    int maskTOP1 = 0b00110000;  // mascara para obtener el primer operando
    int maskTOP2 = 0b11000000;  // mascara para obtener el segundo operando

    if (vm->reg[KS] != -1){
        i = 0;
        while(i < vm->tableSeg[vm->reg[KS] >> 16].size) { 
            printf("[%04X]: ", vm->tableSeg[vm->reg[KS] >> 16].base + i);
            char c = vm->mem[vm->tableSeg[vm->reg[KS] >> 16].base + i];
            while (c != 0 && i < vm->tableSeg[vm->reg[KS] >> 16].size) {
                if (c >= 32 && c <= 126)  // caracteres imprimibles
                    printf("%c", c);
                else
                    printf(".");
                i++;
                c = vm->mem[vm->tableSeg[vm->reg[KS] >> 16].base + i];
            }
            printf("\n");
            i++;
        }
    }

    printf(">");

    while (ip < vm->tableSeg[segmento].size) {
        unsigned char instruction, registro[4], codigoRegistro, aux[4];
        int operando1, operando2, tamanio1, tamanio2, secR1, secR2;
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
        for (; i < 6; i++)
            printf("   ");
        printf("  |  %-8s ", MNEMONIC_NAMES[opc]);

        if (TOP2 == 1) {
            operando2 = vm->mem[ip];
            secR2 = (operando1 >> 6) & 0x3;
            ip++;
        } else if (TOP2 == 2) {
            operando2 = (vm->mem[ip] << 8) | vm->mem[ip + 1];
            operando2 = signExtend(operando2, 2);
            ip += 2;
        } else if (TOP2 == 3) {
            operando2 = (vm->mem[ip] << 16) | (vm->mem[ip + 1] << 8) | vm->mem[ip + 2];
            tamanio2 = (operando2 >> 22) & 0x03;  // los 2 bits más significativos indican el tamaño
            ip += 3;
        } else
            operando2 = 0;

        if (TOP1 == 1) {
            secR2 = (operando2 >> 6) & 0x03;
            operando1 = vm->mem[ip];
            ip++;
        } else if (TOP1 == 2) {
            operando1 = (vm->mem[ip] << 8) | vm->mem[ip + 1];
            ip += 2;
        } else if (TOP1 == 3) {
            operando1 = (vm->mem[ip] << 16) | (vm->mem[ip + 1] << 8) | vm->mem[ip + 2];
            tamanio1 = (operando1 >> 22) & 0x03;
            ip += 3;
        } else
            operando1 = 0;

        // Operandos alineados
        int printed = 0;
        if (TOP1 == 3) {
            unsigned char operandoMemoria = (operando1 & 0xFF000000) >> 24;
            unsigned char codigoRegistro = ((operando1 & 0x1F0000) >> 16);
            unsigned short int offset = operando1 & 0x00FFFF;
            if(tamanio1 == 0)
                printf("l");
            else
                if(tamanio1 == 2)
                    printf("w");
                else
                    if(tamanio1 == 3)
                        printf("b");
            if (offset == 0)
                printf("[%s]", REGISTER_NAMES[codigoRegistro]);
            else
                printf("[%s + %u]", REGISTER_NAMES[codigoRegistro], offset);
            printed = 1;
        } else if (TOP1 == 2) {
            printf("%d", operando1);
            printed = 1;
        } else if (TOP1 == 1) {
            if(secR1 == 0)
                printf("%s", REGISTER_NAMES[codigoRegistro]);
            else{
                strcpy(registro, REGISTER_NAMES[codigoRegistro]);
                strcpy(aux, (unsigned char *) registro[1]);
                if(secR1 == 1)
                    strcat(aux, "L");
                else
                    if(secR1 == 2)
                        strcat(aux, "H");
                    else
                        strcat(aux, "X");
                printf("%s", aux);
            }
            printed = 1;
        }
        if ((TOP1 != 0) && (TOP2 != 0)) {
            printf(", ");
            printed = 1;
        }
        if (TOP2 == 3) {
            unsigned char codigoRegistro = (operando2 & 0x1F0000) >> 16;
            unsigned short int offset = operando2 & 0x00FFFF;
            if(tamanio1 == 0)
                printf("l");
            else
                if(tamanio1 == 2)
                    printf("w");
                else
                    if(tamanio1 == 3)
                        printf("b");
            if (offset == 0)
                printf("[%s]", REGISTER_NAMES[codigoRegistro]);
            else
                printf("[%s + %u]", REGISTER_NAMES[codigoRegistro], offset);
            printed = 1;
        } else if (TOP2 == 2) {
            printf("%d", operando2);
            printed = 1;
        } else if (TOP2 == 1) {
             if(secR2 == 0)
                printf("%s", REGISTER_NAMES[codigoRegistro]);
            else{
                strcpy(registro, REGISTER_NAMES[codigoRegistro]);
                strcpy(aux, (unsigned char *) registro[1]);
                if(secR2 == 1)
                    strcat(aux, "L");
                else
                    if(secR2 == 2)
                        strcat(aux, "H");
                    else
                        strcat(aux, "X");
                printf("%s", aux);
            }
            printed = 1;
        }
        if (!printed)
            printf(" ");
        printf("\n");
    }
}