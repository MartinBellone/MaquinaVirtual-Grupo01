#include "instr_sys.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "VM_memory.h"
#include "constants.h"

// void calculateValueToShow(TVM *vm, int tamanioCelda, int cantLecturas) {
//     int mask = 0xFF000000;
//     for (int i = 0; i < cantLecturas; i++) {
//         (vm->reg[MBR] & mask) >> (24 - i * 8);
//         mask >>= 8;
//     }
// }
void muestraDecimal(TVM *vm, int tamanioCelda) {
    int valor = vm->reg[MBR];
    printf("%d ", valor);
}

void muestraCaracter(TVM *vm, int tamanioCelda) {
    int valor;
    int mask = 0xFF000000;
    for (int i = 0; i < tamanioCelda; i++) {
        valor = vm->reg[MBR] & mask >> (24 - i * 8);
        printf("%c ", (char)(valor));
        mask >>= 8;
    }
}

void muestraOctal(TVM *vm, int tamanioCelda) {
    int valor = vm->reg[MBR];

    printf("%o ", valor);
}

void muestraHexadecimal(TVM *vm, int tamanioCelda) {
    int valor = vm->reg[MBR];
    printf("%X ", valor);
}

void muestraBinario(TVM *vm, int tamanioCelda) {
    int valor = vm->reg[MBR];
    for (int i = tamanioCelda * 8 - 1; i >= 0; i--) {
        if (valor & (1u << i)) {
            printf("1");
        } else {
            printf("0");
        }
    }
}

void SYS(TVM *vm, int tipoOp1, int tipoOp2) {
    int call = vm->reg[OP1];
    call = call & 0x000000FF;  // Aislo los 8 bits menos significativos
    int tamanioCelda = (vm->reg[ECX] & 0xFFFF0000) >> 16;
    int cantLecturas = vm->reg[ECX] & 0x0000FFFF;
    if (vm->reg[EAX] < 0 || vm->reg[EAX] > 0x1F)  //
        exit(1);
    vm->reg[MAR] = tamanioCelda << 16;  // Cargo MAR con la cantidad de bytes a leer

    vm->reg[MAR] &= 0xFFFF0000;
    // Valor Bit Formato
    // 0x10 4 1: interpreta binario
    // 0x08 3 1: interpreta hexadecimal
    // 0x04 2 1: interpreta octal
    // 0x02 1 1: interpreta caracteres
    // 0x01 0 1: interpreta decimal
    void (*func[])(TVM *vm, int cantLecturas) = {
        muestraDecimal,      // 0
        muestraCaracter,     // 1
        muestraOctal,        // 2
        muestraHexadecimal,  // 3
        muestraBinario       // 4
    };

    if (call == 1) {
        // Cargo LAR con la direccion de memoria a escribir
        for (int j = 0; j < cantLecturas; j++) {
            vm->reg[LAR] = vm->reg[EDX] + j * tamanioCelda;
            if (vm->reg[EAX] == 1) {
                char valor;
                printf("[%04X]: ", convertToPhysicalAddress(vm));
                scanf("%d", &valor);
                vm->reg[MBR] = valor;
                writeMemory(vm);
            } else if (vm->reg[EAX] == 2) {
                int valor;
                scanf("%c", &valor);
                vm->reg[MBR] = valor;
                writeMemory(vm);
            } else if (vm->reg[EAX] == 4) {
                int valor;
                scanf("%o", &valor);
                vm->reg[MBR] = valor;
                writeMemory(vm);
            } else if (vm->reg[EAX] == 8) {
                int valor;
                scanf("%x", &valor);
                vm->reg[MBR] = valor;
                writeMemory(vm);
            } else if (vm->reg[EAX] == 16) {
                int value = 0;
                char buffer[33];  // soporta hasta 32 bits
                scanf("%32s", buffer);
                for (int i = 0; i < (int)strlen(buffer); i++) {
                    char c = buffer[i];
                    if (c == '0' || c == '1') {
                        value = (value << 1) | (c - '0');
                    } else {
                        exit(1);
                    }
                }
                if (buffer[0] == '1') {
                    // Ancho usado = len bits
                    int shift = 32 - strlen(buffer);
                    // Extiende signo
                    value = (int)(value << shift) >> shift;
                }
                vm->reg[MBR] = value;
                writeMemory(vm);
            } else
                // No es una base valida
                exit(1);
        }
    } else if (call == 2) {
        int mask = 0xFF;
        for (int j = 0; j < cantLecturas; j++) {
            // Recorro bit a bit los 5 bits menos significativos del registro EAX
            mask = 0b1;
            vm->reg[LAR] = vm->reg[EDX] + j * tamanioCelda;
            vm->reg[MAR] = tamanioCelda << 16;
            readMemory(vm);
            printf("[%04X]: ", vm->reg[MAR] & 0x0000FFFF);
            for (int i = 0; i < 5; i++) {
                if (mask & vm->reg[EAX])
                    func[i](vm, tamanioCelda);
                mask <<= 1;
            }
            printf("\n");
        }
    } else
        exit(1);
}

void STOP(TVM *vm, int tipoOp1, int tipoOp2) {
    exit(0);
}
void invalidOpCode(TVM *vm, int tipoOp1, int tipoOp2) {
    printf("Error: Invalid Mnemonic Code");
    exit(1);
}