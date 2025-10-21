#include "instr_sys.h"
#include "utils.h"
#include "VirtualMachine.h"
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
    unsigned int valor;
    unsigned int mask = 0xFF000000;
    if (tamanioCelda < 4)
        mask >>= (8 * (4 - tamanioCelda));
    // printf("MBR: 0x%08X\n", vm->reg[MBR]);
    for (unsigned int i = 0; i < tamanioCelda; i++) {
        valor = (vm->reg[MBR] & mask) >> (8 * (tamanioCelda - 1) - i * 8);
        if (valor < 32 || valor >= 127)
            printf(".");
        else
            printf("%c", (char)(valor));
        mask >>= 8;
    }
    printf(" ");
}

void muestraOctal(TVM *vm, int tamanioCelda) {
    int valor = vm->reg[MBR];

    printf("0o%o ", valor);
}

void muestraHexadecimal(TVM *vm, int tamanioCelda) {
    int valor = vm->reg[MBR];
    printf("0x%X ", valor);
}

void muestraBinario(TVM *vm, int tamanioCelda) {
    int valor = vm->reg[MBR];
    printf("0b");
    for (int i = tamanioCelda * 8 - 1; i >= 0; i--) {
        if (valor & (1u << i)) {
            printf("1");
        } else {
            printf("0");
        }
    }
    printf(" ");
}

void invalidSysCall(TVM *vm, int tipoOp1, int tipoOp2) {
    exit(1);
}

void SYS1(TVM *vm, int cantLecturas, int tamanioCelda) {
        // Cargo LAR con la direccion de memoria a escribir
        for (int j = 0; j < cantLecturas; j++) {
            vm->reg[LAR] = vm->reg[EDX] + j * tamanioCelda;
            vm->reg[MAR] = tamanioCelda << 16;

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
}

void SYS2(TVM *vm, int cantLecturas, int tamanioCelda){
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
    unsigned int mask = 0xFF;
        for (int j = 0; j < cantLecturas; j++) {
            // Recorro bit a bit los 5 bits menos significativos del registro EAX
            vm->reg[LAR] = vm->reg[EDX] + j * tamanioCelda;
            vm->reg[MAR] = tamanioCelda << 16;
            readMemory(vm);
            mask = 0b10000;

            printf("[%04X]: ", vm->reg[MAR] & 0x0000FFFF);
            for (int i = 4; i >= 0; i--) {
                if (mask & vm->reg[EAX])
                    func[i](vm, tamanioCelda);
                mask >>= 1;
            }
            printf("\n");
        }
}

void SYS3(TVM *vm, int cantMaximaCaracteres, int tamanioCelda){
    unsigned char *cadena;
    unsigned int i, segmento, tamanioSegmento;
    segmento = vm->reg[EDX] >> 16;
    tamanioSegmento = vm->tableSeg[segmento].size;
    if (vm->tableSeg[segmento].size < cantMaximaCaracteres){
        printf("Error: Segmentation fault\n");
        exit(1);
    }
    if(cantMaximaCaracteres == -1)
        cadena = (unsigned char *)malloc((tamanioSegmento) * sizeof(unsigned char));
    else
        cadena = (unsigned char *)malloc((cantMaximaCaracteres + 1) * sizeof(unsigned char));
    scanf("%s", cadena);
    for (i = 0; cadena[i] != '\0'; i++) {
        vm->reg[LAR] = vm->reg[EDX] + i;
        if((vm->reg[LAR] & 0xFFFF) > tamanioSegmento){
            printf("Error: Segmentation fault\n");
            exit(1);
        }
        vm->reg[MAR] = tamanioCelda << 16;
        vm->reg[MBR] = cadena[i];
        writeMemory(vm);
    }
    vm->reg[LAR] = vm->reg[EDX] + i;
    if((vm->reg[LAR] & 0xFFFF) > tamanioSegmento){
        printf("Error: Segmentation fault\n");
        exit(1);
    }
    vm->reg[MAR] = tamanioCelda << 16;
    vm->reg[MBR] = '\0';
    writeMemory(vm);
    free(cadena);
}

void SYS4(TVM *vm, int tipoOp1, int tipoOp2){
    unsigned char c;
    unsigned int i, segmento, tamanioSegmento;
    segmento = vm->reg[EDX] >> 16;
    i = 0;
    do{
        vm->reg[LAR] = vm->reg[EDX] + i * 1;
        tamanioSegmento = vm->tableSeg[segmento].size;
        if((vm->reg[LAR] & 0xFFFF) > tamanioSegmento){
            printf("Error: Segmentation fault\n");
            exit(1);
        }
        vm->reg[MAR] = 0x00010000; // leo 1 byte
        readMemory(vm);
        c = (unsigned char)(vm->reg[MBR] & 0x000000FF);
        printf("%c", c);
        i++;
    } while(c != '\0');
}

void SYS7(TVM *vm, int sinUso1, int sinUso2){
    fflush(stdout);
}
    
void SYSF(TVM *vm, int sinUso, int SinUso2){
    writeFile(vm, vm->vmiFile);
    char c;

    scanf("%c",&c); // Espero a que el usuario presione una tecla para finalizar
    switch (tolower(c)) {
    case 'q': exit(0); break; // Salgo del programa
    case '\n': while (c == '\n'){ // Si el usuario presiona enter, sigo mostrando la ejecucion paso a paso
                 readInstruction(vm);
                 scanf("%c",&c);
               }
                break;
    case 'g': // Si el usuario presiona g, sigo la ejecucion normal
                break;
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

    void (*func[])(TVM *vm, int cantLecturas, int tamanioCelda) = {
        invalidSysCall,  // 0
        SYS1,           // 1
        SYS2,           // 2
        SYS3,           // 3
        SYS4,           // 4
        invalidSysCall,  // 5
        invalidSysCall,  // 6
        SYS7,           // 7
        invalidSysCall,  // 8
        invalidSysCall,  // 9
        invalidSysCall,  // A
        invalidSysCall,  // B
        invalidSysCall,  // C
        invalidSysCall,  // D
        invalidSysCall,  // E
        SYSF            // F
    };

    if(vm->reg[EDX] < 0)
        exit(1);

    if(call < 0 || call > 0x0F) // Si el valor de call es mayor a 15 o menor a 0, salgo con error
        exit(1);
    
    func[call](vm, cantLecturas, tamanioCelda);
}

void STOP(TVM *vm, int tipoOp1, int tipoOp2) {
    vm->reg[IP] = 0xFFFFFFFF;  // Pongo IP en una direccion invalida para que no siga ejecutando
}