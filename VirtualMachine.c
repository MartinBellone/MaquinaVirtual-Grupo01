#include "VirtualMachine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LAR 0 
#define MAR 1
#define MBR 2
#define IP 3 //registro del contador de instrucciones
#define OPC 4
#define OP1 5
#define OP2 6
#define EAX 10
#define EBX 11
#define ECX 12
#define EDX 13
#define EEX 14
#define EFP 15
#define AC 16  
#define CC 17              
#define CS 26
#define DS 27

int convertToPhysicalAddress(TVM *vm){
    int segment, baseSeg, offSeg;
    segment = vm->reg[LAR] & 0xFFFF0000;
    baseSeg = vm->tableSeg[segment].base;
    offSeg = vm->reg[LAR] & 0x0000FFFF;
    return baseSeg + offSeg;
}

void readMemory(TVM *vm){
    int physAddr = convertToPhysicalAddress(vm);
    vm->reg[MAR] = physAddr;
    vm->reg[MBR] = vm->mem[MAR];
}

void writeMemory(TVM *vm){
    int physAddr = convertToPhysicalAddress(vm);

}

void initTSR(TVM * vm, char *size){
    vm->tableSeg[0].base=0;
    strcpy(vm->tableSeg[0].size,size);
    strcpy(vm->tableSeg[1].size,size);
    int cantBytes = atoi(size);
    vm->tableSeg[1].size = 16384 - cantBytes;
}

void readFile(TVM *vm, char name[]){     //funcion para leer el vmx
    FILE *arch;
    int i=0; //direccion de memoria a guardar el byte
    char c, size[2], header[8];

    arch=fopen(name,"rb");
    if (arch==NULL)
         printf("ERROR al abrir el archivo");
    else{
        // Lectura del identificador y version del archivo
        fread(header, 5*sizeof(char), 5, arch);
        if(strcmp(header, "VMX251") == 0){
            // Leo tamaño del archivo
            fread(size, 2*sizeof(char), 2, arch);

            // Inicializo tabla de segmentos
            initTSR(vm, size);

            if (vm->tableSeg[1].size <= 0){  //si se supera el tamaño del segmento de codigo, salir
                 printf("Error: El programa es demasiado grande para la memoria asignada.\n");
                }
            else{
                 // Leer codigo
                while (fread(&c,sizeof(char),1,arch)==1){ 
                    vm->mem[i]=c;
                    i++;
                }
                initVm(vm);
               
            }
        }
        else
          printf("Error: Formato de archivo incorrecto.\n");
        fclose(arch);
    } 
    //TODO cambiar los prints y revisar en general
}
void initVm(TVM *vm){
    vm->reg[CS]=0;  //segmento de codigo
    vm->reg[IP]= vm->reg[CS];  //contador de instrucciones apunta al inicio del segmento de codigo
    vm->reg[DS]= 1 << 16;  //segmento de datos
}
void readInstruction(TVM *vm){
    char instruction;
    int maskOPC=0b00011111;  //mascara para obtener el codigo de operacion
    int maskTOP1=0b00110000;  //mascara para obtener el primer operando
    int maskTOP2=0b11000000;  //mascara para obtener el segundo operando
    int TOP1,TOP2;
    instruction= vm->mem[vm->reg[IP]];

    TOP2= (instruction & maskTOP2) >> 6;
    TOP1= (instruction & maskTOP1) >> 4;
    vm->reg[OPC]= instruction & maskOPC;

    // vm->reg[OP2] = TOP2 << 24;
    // if (TOP2==0b11) {//carga operando 2 (memoria)
    //    //carga en el byte mas significativo con el tipo de operando
    //    readOp(vm,TOP2,OP2);
    // }
    // else{ //lee operandos de code segment
    //     vm->reg[OP2] = TOP2 << 24; //carga en el byte mas significativo con el tipo de operando
    //     readOp(vm,TOP2,OP2);
    // }

    // vm->reg[OP1] = TOP1 << 24; //carga en el byte mas significativo con el tipo de operando
    // if (TOP1==0b11) //carga operando 1
    //     vm->reg[OP1] = readMemory(/*Completar*/); //da error por el tipo de la funcion
    // else{
    //     readOp(vm,TOP1,OP1);
    // }
    //TODO puntero a funcion segun OPC
}
void readOp(TVM *vm,int TOP, int numOp){ //numOp es OP1 u OP2

    vm->reg[numOp] = TOP << 24; //carga en el byte mas significativo con el tipo de operando
    if (TOP == 0b11){ //memoria
        vm->reg[numOp] = (vm->mem[vm->reg[IP]+1] << 16) | vm->mem[vm->reg[IP]+2] >> 8 | vm->mem[vm->reg[IP]+3]; //lee la direccion de memoria
        vm->reg[IP]+=3; //incrementa el contador de instrucciones
        
    }
    if (TOP == 0b01){ //registro
        vm->reg[numOp] = vm->mem[vm->reg[IP]+1]; //lee el registro
        vm->reg[IP]++; //incrementa el contador de instrucciones
    }
    else{ //inmediato
        vm->reg[numOp] = (vm->mem[vm->reg[IP]+1] << 8) | vm->mem[vm->reg[IP]+2]; //lee el inmediato
        vm->reg[IP]+=2; //incrementa el contador de instrucciones
    }
}

void MOV(TVM *vm, int tipoOp1, int tipoOp2){
    int mask=0x00FFFFFF;
    int opAux;
    opAux = vm->reg[OP2] & mask; //obtengo el operando sin el tipo
    opAux = (opAux << 8) >> 8; //extiendo el signo
    vm->reg[vm->reg[OP1] & mask] = opAux; //muevo el valor al registro destino
}

