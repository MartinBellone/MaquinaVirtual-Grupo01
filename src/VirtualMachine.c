#include "VirtualMachine.h"

#include <stdint.h>
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

void menu(TVM* vm, int tipoOp1, int tipoOp2) {
    void (*func[])(TVM* vm, int tipoOp1, int tipoOp2) = {
        SYS, JMP, JZ, JP, JN, JNZ, JNP, JNN, NOT, invalidOpCode, invalidOpCode, PUSH,
        POP, CALL, RET,
        STOP, MOV, ADD, SUB, MUL, DIV, CMP, SHL, SHR, SAR, AND, OR, XOR, SWAP, LDL, LDH,
        RND};
    func[vm->reg[OPC]](vm, tipoOp1, tipoOp2);
}

void initTSR(TVM* vm, unsigned short int sizes[7], unsigned short int cantSegments) {
    for (int i = 0; i < 8; i++) {
        vm->tableSeg[i].base = 0;
        vm->tableSeg[i].size = 0;
    }
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

void parseArgs(int argc, char* argv[], VMParams* argsSalida, TVM* vm) {
    argsSalida->vmxFile = NULL;
    argsSalida->vmiFile = NULL;
    argsSalida->memSize = 16 * 1024;  // default 16KiB
    argsSalida->disassembly = 0;
    argsSalida->argc = 0;

    for (int i = 1; i < argc; i++) {
        printf("Argumento %d: %s\n", i, argv[i]);
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
                // printf("Agregando parametro: %s\n", argv[j]);
                argsSalida->argv[argsSalida->argc++] = strdup(argv[j]);
                // strcpy(argsSalida->argv[argsSalida->argc++], argv[j]);
            }
            break;
        } else {
            printf("Argumento desconocido: %s\n", argv[i]);
        }
    }

    if (argsSalida->vmiFile) {
        vm->vmiFile = argsSalida->vmiFile;
    }
}
void buildParamSegment(TVM* vm, VMParams* argsSalida) {
    unsigned int base = 0;  // El segmento de parámetros siempre inicia en 0
    unsigned int offset = 0;
    unsigned int ptrs[argsSalida->argc];  // direcciones de cada string

    if (argsSalida->argc == 0) {
        vm->reg[PS] = -1;
        // printf("No hay parámetros para el segmento PS.\n");
        return;
    }

    // Copiar los strings uno detrás del otro
    for (int i = 0; i < argsSalida->argc; i++) {
        ptrs[i] = 0x00000000 | offset;  // dirección del inicio del string i
        // printf("Entro al ciclo %d\n", i);
        int len = strlen(argsSalida->argv[i]) + 1;  // incluye el '\0'
        memcpy(&vm->mem[offset], argsSalida->argv[i], len);
        // free(argsSalida->argv[i]);  // liberar memoria del string copiado
        offset += len;
    }

    vm->argc = argsSalida->argc;
    // Agregar marcador de fin de punteros (-1)
    ptrs[argsSalida->argc] = 0xFFFFFFFF;

    // Copiar los punteros al final de la sección de strings
    vm->argv = offset;  // dirección del inicio del arreglo de punteros
    for (int i = 0; i <= argsSalida->argc; i++) {
        // memcpy(&vm->mem[offset], &ptrs[i], sizeof(int));
        // offset += sizeof(int);
        vm->mem[offset] = (ptrs[i] >> 24) & 0xFF;
        vm->mem[offset + 1] = (ptrs[i] >> 16) & 0xFF;
        vm->mem[offset + 2] = (ptrs[i] >> 8) & 0xFF;
        vm->mem[offset + 3] = ptrs[i] & 0xFF;
        offset += 4;
    }
    // Configurar el segmento en la tabla
    vm->tableSeg[0].base = base;
    vm->tableSeg[0].size = offset;

    // Registrar el segmento PS
    vm->reg[PS] = base;

    // for (int i = 0; i < argsSalida->argc; i++) {
    //     printf("Parametro %d: %02X (ptr: %08X)\n", i, vm->mem[ptrs[i]], ptrs[i]);
    // }
}

void readFileVMX(TVM* vm, char* fileName) {
    // funcion para leer el vmx
    FILE* arch;
    unsigned char c, header[6], version;
    unsigned int CSsize;
    unsigned short int sizes[7] = {0};  // tamanos de los segmentos

    arch = fopen(fileName, "rb");
    if (arch == NULL) {
        printf("ERROR al abrir el archivo \"%s\"\n", fileName);
        exit(1);
    } else {
        // Lectura del identificador y version del archivo
        fread(header, sizeof(char), 5, arch);
        header[5] = '\0';  // Asegurarse de que la cadena esté terminada en null
        if (strcmp(header, (unsigned char*)"VMX25") != 0) {
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
            // printf("Sizes - PS: %u, CS: %u, DS: %u, ES: %u, SS: %u, KS: %u\n", vm->tableSeg[0].size, CSsize, DSsize, ESsize, SSsize, KSsize);
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

            if (vm->reg[PS] == -1)
                sizes[0] = 0;
            else
                sizes[0] = vm->tableSeg[0].size;
            sizes[1] = KSsize;      // KS
            sizes[2] = CSsize;      // CS
            sizes[3] = DSsize;      // DS
            sizes[4] = ESsize;      // ES
            sizes[5] = SSsize;      // SS
            sizes[6] = entryPoint;  // entry point
            initTSR(vm, sizes, 6);
            sizes[0] = CSsize;  // CS
            sizes[1] = DSsize;  // DS
            sizes[2] = ESsize;  // ES
            sizes[3] = SSsize;  // SS
            sizes[4] = KSsize;  // KS
            if (vm->reg[PS] == -1)
                sizes[5] = 0;
            else
                sizes[5] = vm->tableSeg[0].size;  // PS
            sizes[6] = entryPoint;                // entry point
            // printf("Entry point: %x\n", entryPoint);
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

        if (version == 2 && vm->reg[KS] > 0) {
            // debo inicializar el segmento de constantes
            i = vm->tableSeg[KSsegment].base;
            vm->mem[i] = c;  // leo el byte que quedo pendiente
            i++;
            while (fread(&c, sizeof(char), 1, arch) == 1) {
                vm->mem[i] = c;
                i++;
            }
        }
        fclose(arch);
    }
}

void readFileVMI(TVM* vm, char* fileName) {
    // Lee archivos .vmi según la especificación:
    // Header (8 bytes): "VMI25" (5), versión (1), tamaño de memoria en KiB (2, big-endian)
    // Registros (32 x 4 bytes, big-endian)
    // Tabla de segmentos (8 x 4 bytes: base[2], size[2], big-endian)
    // Memoria principal (memKiB * 1024 bytes)
    FILE* arch;
    unsigned char header[6], version;
    unsigned short memKiB;
    size_t memBytes;
    unsigned short int base, size;
    unsigned char buf[4];

    arch = fopen(fileName, "rb");
    if (arch == NULL) {
        printf("ERROR al abrir el archivo %s : ", fileName);
        return;
    }

    // Header
    if (fread(header, 1, 5, arch) != 5) {
        printf("ERROR: no se pudo leer el identificador del header\n");
        fclose(arch);
        exit(1);
    }
    header[5] = '\0';
    if (strcmp((char*)header, "VMI25") != 0) {
        printf("ERROR: formato de archivo incorrecto (header %s)\n", header);
        fclose(arch);
        exit(1);
    }

    if (fread(&version, 1, 1, arch) != 1) {
        printf("ERROR: no se pudo leer la versión del archivo\n");
        fclose(arch);
        exit(1);
    }
    if (version != 0x01) {
        printf("ERROR: versión de archivo incorrecta (%d)\n", version);
        fclose(arch);
        exit(1);
    }

    // Leer memKiB en big-endian (2 bytes)
    if (fread(buf, 1, 2, arch) != 2) {
        printf("ERROR: no se pudo leer el tamaño de la memoria (KiB)\n");
        fclose(arch);
        exit(1);
    }
    memKiB = (buf[0] << 8) | buf[1];
    memBytes = (size_t)memKiB * 1024;

    // Registros (big-endian, 4 bytes cada uno)
    for (int i = 0; i < 32; i++) {
        if (fread(buf, 1, 4, arch) != 4) {
            printf("ERROR: no se pudo leer el registro %d\n", i);
            fclose(arch);
            exit(1);
        }
        vm->reg[i] = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
    }

    // Tabla de segmentos (big-endian, 2 bytes base + 2 bytes size)
    for (int i = 0; i < 8; i++) {
        if (fread(buf, 1, 2, arch) != 2) {
            printf("ERROR: no se pudo leer la base del segmento %d\n", i);
            fclose(arch);
            exit(1);
        }
        base = (buf[0] << 8) | buf[1];

        if (fread(buf, 1, 2, arch) != 2) {
            printf("ERROR: no se pudo leer el tamaño del segmento %d\n", i);
            fclose(arch);
            exit(1);
        }
        size = (buf[0] << 8) | buf[1];

        vm->tableSeg[i].base = base;
        vm->tableSeg[i].size = size;
    }

    // Memoria
    vm->mem = (unsigned char*)malloc(memBytes);
    if (!vm->mem) {
        printf("ERROR: no se pudo asignar memoria (%zu bytes)\n", memBytes);
        fclose(arch);
        exit(1);
    }
    if (fread(vm->mem, 1, memBytes, arch) != memBytes) {
        printf("ERROR: no se pudo leer la memoria principal (%zu bytes)\n", memBytes);
        fclose(arch);
        exit(1);
    }

    fclose(arch);
}

void writeFileVMI(TVM* vm, char* fileName) {
    // Escribe archivos .vmi según la especificación en big-endian:
    // Header (8 bytes): "VMI25" (5), versión (1), tamaño memoria en KiB (2)
    // Registros (32 x 4 bytes, big-endian)
    // Tabla de segmentos (8 x 4 bytes: base[2], size[2], big-endian)
    // Memoria principal (memKiB*1024 bytes)
    FILE* arch;
    unsigned char header[6] = "VMI25";
    unsigned char version = 0x01;
    unsigned char buf[4];

    // Calcular tamaño total de memoria desde la tabla de segmentos
    size_t totalBytes = 0;
    for (int i = 0; i < 8; i++) {
        totalBytes += vm->tableSeg[i].size;
    }
    unsigned short memKiB = (unsigned short)((totalBytes + 1023) / 1024);  // redondeo hacia arriba
    size_t memBytes = (size_t)memKiB * 1024;

    arch = fopen(fileName, "wb+");
    if (arch == NULL) {
        return;
    }

    // Header
    fwrite(header, 1, 5, arch);
    fwrite(&version, 1, 1, arch);

    // Escribir memKiB en big-endian (2 bytes)
    buf[0] = (memKiB >> 8) & 0xFF;
    buf[1] = memKiB & 0xFF;
    fwrite(buf, 1, 2, arch);

    // Registros (big-endian, 4 bytes cada uno)
    for (int i = 0; i < 32; i++) {
        buf[0] = (vm->reg[i] >> 24) & 0xFF;
        buf[1] = (vm->reg[i] >> 16) & 0xFF;
        buf[2] = (vm->reg[i] >> 8) & 0xFF;
        buf[3] = vm->reg[i] & 0xFF;
        fwrite(buf, 1, 4, arch);
    }

    // Tabla de segmentos (big-endian, 2 bytes base + 2 bytes size)
    for (int i = 0; i < 8; i++) {
        // Base (2 bytes, big-endian)
        buf[0] = (vm->tableSeg[i].base >> 8) & 0xFF;
        buf[1] = vm->tableSeg[i].base & 0xFF;
        fwrite(buf, 1, 2, arch);

        // Size (2 bytes, big-endian)
        buf[0] = (vm->tableSeg[i].size >> 8) & 0xFF;
        buf[1] = vm->tableSeg[i].size & 0xFF;
        fwrite(buf, 1, 2, arch);
    }

    // Memoria (rellenando con 0 si es necesario para completar KiB)
    if (vm->mem && totalBytes > 0) {
        size_t toWrite = totalBytes;
        if (toWrite > memBytes) toWrite = memBytes;  // seguridad
        fwrite(vm->mem, 1, toWrite, arch);
        // Relleno
        size_t pad = (memBytes > toWrite) ? (memBytes - toWrite) : 0;
        if (pad) {
            // Escribir ceros en bloques pequeños para evitar usar mucha memoria
            unsigned char zeroBuf[256] = {0};
            while (pad > 0) {
                size_t chunk = pad > sizeof(zeroBuf) ? sizeof(zeroBuf) : pad;
                fwrite(zeroBuf, 1, chunk, arch);
                pad -= chunk;
            }
        }
    } else if (memBytes > 0) {
        // No hay memoria cargada pero la especificación exige memBytes
        unsigned char zeroBuf[256] = {0};
        size_t pad = memBytes;
        while (pad > 0) {
            size_t chunk = pad > sizeof(zeroBuf) ? sizeof(zeroBuf) : pad;
            fwrite(zeroBuf, 1, chunk, arch);
            pad -= chunk;
        }
    }

    fclose(arch);
}

void initVm(TVM* vm, unsigned short int sizes[7], unsigned short int cantSegments) {
    unsigned short int totalSize = 0;
    int value;
    int j = 0;
    // Necesito tratar los registros PS y KS por separado
    if (sizes[5] != 0) {
        vm->reg[PS] = j << 16;
        j++;
        totalSize += sizes[5];
        if (sizes[4] != 0) {
            vm->reg[KS] = j << 16;
            j++;
            totalSize += sizes[4];
        }

    } else {
        if (sizes[4] != 0) {
            vm->reg[KS] = j << 16;
            j++;
            totalSize += sizes[4];
        }
    }

    cantSegments -= j;  // resto los segmentos ya asignados
    for (int i = 0; i <= cantSegments; i++) {
        if (sizes[i] != 0) {
            vm->reg[26 + i] = j << 16;
            j++;
        } else
            vm->reg[26 + i] = -1;  // segmento no usado
        totalSize += sizes[i];
        // printf("Registro %d inicializado con valor %x\n", 26 + i, vm->reg[26 + i]);
    }
    unsigned char* oldMem = vm->mem;
    vm->mem = (unsigned char*)malloc(totalSize * sizeof(unsigned char));
    if (oldMem) {
        // Copiar el contenido del segmento de parámetros si existía
        unsigned int paramSize = vm->tableSeg[0].size;
        memcpy(vm->mem, oldMem, paramSize);
        vm->mem[paramSize] = 0;  // Asegurar que el siguiente byte esté limpio
        free(oldMem);
    }
    // printf("Memoria de %d bytes asignada a la VM.\n", totalSize);
    if (vm->reg[SS] != -1) {
        unsigned int codeSegment = vm->reg[CS] >> 16;
        vm->reg[IP] = codeSegment;

        // inicializo el instruction pointer en la parte alta con con la direccion del CS y en la parte baja con el entry point
        vm->reg[IP] = vm->reg[IP] << 16;
        vm->reg[IP] |= sizes[6];  // entry point
        // printf("Instruction Pointer inicializado en %08X\n", vm->reg[IP]);       // entry point
        vm->reg[SP] = vm->reg[SS] | (vm->tableSeg[vm->reg[SS] >> 16].size - 1);  // inicializo el stack pointer en la parte alta del segmento de stack

        // Inicializo Stack
        vm->reg[SP] -= 3;
        value = vm->argv;

        vm->reg[LAR] = vm->reg[SP];  // cargo el registro LAR con el stack segment
        unsigned int physAddr = convertToPhysicalAddress(vm);

        vm->mem[physAddr] = (value >> 24) & 0xFF;      // byte más significativo
        vm->mem[physAddr + 1] = (value >> 16) & 0xFF;  // segundo byte
        vm->mem[physAddr + 2] = (value >> 8) & 0xFF;   // tercer byte
        vm->mem[physAddr + 3] = value & 0xFF;          // byte menos significativo

        vm->reg[SP] -= 4;
        value = vm->argc;
        vm->reg[LAR] = vm->reg[SP];  // cargo el registro LAR con el stack segment
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
    } else {
        vm->reg[IP] = 0;
    }
}
void readOp(TVM* vm, int TOP, int numOp) {  // numOp es OP1 u OP2 y TOP tipo de operando
    unsigned int physAddr;
    vm->reg[LAR] = vm->reg[IP];  // cargo el registro LAR con el instruction pointer
    physAddr = convertToPhysicalAddress(vm);
    if (TOP == 0b01) {
        vm->reg[numOp] = 0x01 << 24;          // registro
        vm->reg[numOp] |= vm->mem[physAddr];  // lee el registro
        vm->reg[IP]++;                        // incrementa el contador de instrucciones
    } else {
        if (TOP == 0b10) {
            // inmediato: 2 bytes (16 bits). Encode as: TT ........ where TT=0x02 in the top byte
            unsigned int high = vm->mem[physAddr];
            unsigned int low = vm->mem[physAddr + 1];
            unsigned int imm = (high << 8) | low;
            imm = signExtend(imm, 2);
            // Store with type in top byte (0x02)
            vm->reg[numOp] = (0x02 << 24) | (imm & 0x00FFFFFF);
            vm->reg[IP] += 2;  // incrementa el contador de instrucciones
        } else {
            if (TOP == 0b11) {                                                                                       // memoria
                vm->reg[numOp] = 0x03 << 24;                                                                         // memoria
                vm->reg[numOp] |= (vm->mem[physAddr] << 16) | (vm->mem[physAddr + 1] << 8) | vm->mem[physAddr + 2];  // lee el operando de memoria
                vm->reg[IP] += 3;                                                                                    // incrementa el contador de instrucciones
            } else {
                printf("Error: Tipo de operando invalido.\n");
                exit(1);
            }
        }
    }
}

void readInstruction(TVM* vm) {
    unsigned char instruction;
    int maskOPC = 0b00011111;   // mascara para obtener el codigo de operacion
    int maskTOP1 = 0b00110000;  // mascara para obtener el primer operando
    int maskTOP2 = 0b11000000;  // mascara para obtener el segundo operando
    int TOP1, TOP2;
    unsigned int segmento = vm->reg[IP] >> 16;
    unsigned int base = vm->tableSeg[segmento].base;
    unsigned int offset = vm->reg[IP] & 0x0000FFFF;
    instruction = vm->mem[base + offset];  // leo la instruccion

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

    // showStackSegment(vm);
    // printf("\n------------------------------\n");
    // printf("Instruccion leida: Mnemonico=%-8s, TOP1=%d, TOP2=%d\n", MNEMONIC_NAMES[vm->reg[OPC]], TOP1, TOP2);
    // printf("OP1=%08X\n", vm->reg[OP1]);
    // printf("OP2=%08X\n", vm->reg[OP2]);

    menu(vm, TOP1, TOP2);
}

void executeProgram(TVM* vm) {
    printf("\n----- Ejecucion del programa -----\n");
    while (1) {
        unsigned int codeSegment = vm->reg[CS] >> 16;

        if ((vm->reg[IP] & 0x0000FFFF) >= (vm->tableSeg[codeSegment].size + vm->tableSeg[codeSegment].base)) {
            printf("Fin de la ejecucion del programa.\n");
            exit(0);
        } else {
            if (vm->reg[IP] == -1)  // instruccion STOP
                exit(0);
        }
        readInstruction(vm);
    }
}

void executeDisassembly(TVM* vm) {
    printf("----- Disassembler -----\n");

    int ip = vm->reg[IP], segmento = ip >> 16, i;
    ip = ip & 0x0000FFFF;
    ip = vm->tableSeg[vm->reg[CS] >> 16].base;
    int maskOPC = 0b00011111;   // mascara para obtener el codigo de operacion
    int maskTOP1 = 0b00110000;  // mascara para obtener el primer operando
    int maskTOP2 = 0b11000000;  // mascara para obtener el segundo operando

    if (vm->reg[KS] != -1) {
        i = 0;
        while (i < vm->tableSeg[vm->reg[KS] >> 16].size) {
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

    // printf("ip: %x\n", ip);
    // printf("%d %d \n", vm->tableSeg[segmento].size, vm->tableSeg[segmento].base);
    while (ip < (vm->tableSeg[segmento].size + vm->tableSeg[segmento].base)) {
        unsigned char instruction, registro[4], codigoRegistro, aux[4];
        int operando1, operando2, tamanio1, tamanio2, secR1, secR2;
        int TOP1, TOP2, opc, i;

        if (ip == ((vm->reg[IP] & 0x0000FFFF) + vm->tableSeg[vm->reg[CS] >> 16].base)) {
            printf("-> ");
        } else {
            printf("   ");
        }

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
            secR2 = (operando2 >> 6) & 0x3;
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
            operando1 = vm->mem[ip];
            secR1 = (operando1 >> 6) & 0x03;
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
        // printf("TOP1=%d TOP2=%d\n", TOP1, TOP2);
        // printf("OP1=%08X \t", operando1);
        // printf("OP2=%08X \t", operando2);
        // printf("\nSecR1=%d, SecR2=%d\t\n", secR1, secR2);
        // Operandos alineados
        int printed = 0;
        if (TOP1 == 3) {
            unsigned char codigoRegistro = ((operando1 & 0x1F0000) >> 16);
            unsigned char operandoMemoria = (operando1 & 0xFF000000) >> 24;
            int offset = (int16_t)(operando1 & 0x00FFFF);
            if (tamanio1 == 0)
                printf("l");
            else if (tamanio1 == 2)
                printf("w");
            else if (tamanio1 == 3)
                printf("b");
            if (offset == 0)
                printf("[%s]", REGISTER_NAMES[codigoRegistro]);
            else if (offset < 0)
                printf("[%s%d]", REGISTER_NAMES[codigoRegistro], offset);
            else
                printf("[%s+%d]", REGISTER_NAMES[codigoRegistro], offset);
            printed = 1;
        } else if (TOP1 == 2) {
            printf("%d", operando1);
            printed = 1;
        } else if (TOP1 == 1) {
            unsigned char codigoRegistro = operando1;
            // printf("Fallo aca!");
            // printf("este es el codigo de registro: %d", codigoRegistro);
            if (secR1 == 0)
                printf("%s", REGISTER_NAMES[codigoRegistro]);
            else {
                strcpy(registro, REGISTER_NAMES[codigoRegistro]);
                strcpy(aux, (unsigned char*)registro[1]);
                if (secR1 == 1)
                    strcat(aux, "L");
                else if (secR1 == 2)
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
            int offset = (int16_t)(operando2 & 0x00FFFF);
            if (tamanio1 == 0)
                printf("l");
            else if (tamanio1 == 2)
                printf("w");
            else if (tamanio1 == 3)
                printf("b");
            if (offset == 0)
                printf("[%s]", REGISTER_NAMES[codigoRegistro]);
            else if (offset < 0)
                printf("[%s%d]", REGISTER_NAMES[codigoRegistro], offset);
            else
                printf("[%s+%d]", REGISTER_NAMES[codigoRegistro], offset);
            printed = 1;
        } else if (TOP2 == 2) {
            printf("%d", operando2);
            printed = 1;
        } else if (TOP2 == 1) {
            unsigned char codigoRegistro = operando2;
            if (secR2 == 0)
                printf("%s", REGISTER_NAMES[codigoRegistro]);
            else {
                strcpy(registro, REGISTER_NAMES[codigoRegistro]);
                strcpy(aux, (unsigned char*)registro[1]);
                if (secR2 == 1)
                    strcat(aux, "L");
                else if (secR2 == 2)
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