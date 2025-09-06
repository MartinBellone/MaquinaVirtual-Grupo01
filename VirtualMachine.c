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
    segment = (vm->reg[LAR] & 0xFFFF0000) >> 16; //obtengo el segmento

    if (segment > 7){ //si el segmento es mayor a 7, error
        printf("Error: Segmentation fault.\n");
        exit(1);
    }

    baseSeg = vm->tableSeg[segment].base;
    offSeg = vm->reg[LAR] & 0x0000FFFF;
    return 0x00000000 | (baseSeg + offSeg);
}

void readMemory(TVM *vm){
    int physAddr = convertToPhysicalAddress(vm);
    vm->reg[MAR] = physAddr;
    vm->reg[MBR] = vm->mem[vm->reg[MAR]];
}

void writeMemory(TVM *vm){
    int physAddr = convertToPhysicalAddress(vm);

}
void createLogicAdress(TVM *vm){
    // int segment, offset;
    // segment = vm->reg[LAR] & 0xFFFF0000; //obtengo el segmento
    // offset = vm->reg[LAR] & 0x0000FFFF; //obtengo el offset
    // vm->reg[LAR] = (vm->reg[DS] & 0xFFFF0000) | offset; //cargo LAR con segmento de datos y offset del operando
}
void initTSR(TVM * vm, char *size){
    vm->tableSeg[0].base=0;
    strcpy(vm->tableSeg[0].size,size);
    strcpy(vm->tableSeg[1].base,size);
    int cantBytes = atoi(size);
    vm->tableSeg[1].size = 16384 - cantBytes;
}

void readFile(TVM *vm,  char *fileName){     //funcion para leer el vmx
    FILE *arch; //TODO arreglar con writeMemory
    int i=0; //direccion de memoria a guardar el byte
    char c, size[2], header[8];

    arch=fopen(fileName,"rb");
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
    int maskOPC=0b00011111;   //mascara para obtener el codigo de operacion
    int maskTOP1=0b00110000;  //mascara para obtener el primer operando
    int maskTOP2=0b11000000;  //mascara para obtener el segundo operando
    int TOP1,TOP2;
    
    vm->reg[LAR] = vm->reg[IP]; //TODO preguntar, es correcto simular la lectura de instruccion
    readMemory(vm);
    instruction = vm->reg[MBR];
    // decodifica la instruccion 
    TOP2= (instruction & maskTOP2) >> 6;
    TOP1= (instruction & maskTOP1) >> 4;
    vm->reg[OPC]= instruction & maskOPC;
    vm->reg[IP]++; //se para en el primer byte del segundo operando
    // lee los operandos
    readOp(vm,OP2,TOP2);
    readOp(vm,OP1,TOP1);
    
    //TODO puntero a funcion segun OPC

}
void readOp(TVM *vm,int TOP, int numOp){ //numOp es OP1 u OP2 y TOP tipo de operando

    vm->reg[numOp] = TOP << 24; //carga en el byte mas significativo con el tipo de operando

    for (int i=1;i<=TOP;i++){ //lee operando byte a byte
        vm->reg[LAR] = vm->reg[IP];
        readMemory(vm); 
        vm->reg[numOp] |=  (vm->reg[MBR] << 8*(TOP - i)); //shiftea segun posicion del byte necesaria
        vm->reg[IP]++;
    }
}

void MOV(TVM *vm, int tipoOp1, int tipoOp2){
    // int mask=0x00FFFFFF;
    // int opAux;
    // opAux = vm->reg[OP2] & mask; //obtengo el operando sin el tipo
    // opAux = (opAux << 8) >> 8; //extiendo el signo
    // vm->reg[vm->reg[OP1] & mask] = opAux; //muevo el valor al registro destino
    
    if (tipoOp1 == 1){ //registro
        if (tipoOp2 == 1){ //registro
            //MOV EDX,EEX
            //01010000 0E 0D
            //OP1 = 0x0100000D OP2 = 0x0100000E
            vm->reg[vm->reg[OP1] & 0x00FFFFFF] = vm->reg[vm->reg[OP2] & 0x00FFFFFF];
        }
        else if (tipoOp2 == 2){ //inmediato
            //0x0200000A
            //0x00000A00
            //0x0000000A
            // el shift a la derecha es aritmético, por lo que extiende el signo
            vm->reg[vm->reg[OP1] & 0x00FFFFFF] = (vm->reg[OP2] << 8) >> 8; //extiendo el signo
        }
        else if (tipoOp2 == 3){ //memoria
            //MOV EAX, [EDX+4]
            //MOV EAX, [EDX]
            //MOV EAX, [4] = MOV EAX, [DS+4]
            // TODO preguntar como vienen los operandos de memoria
            vm->reg[LAR] = (vm->reg[DS] & 0xFFFF0000) | (vm->reg[OP2] & 0x0000FFFF); //cargo LAR con segmento de datos y offset del operando
            readMemory(vm);
            vm->reg[vm->reg[OP1] & 0x00FFFFFF] = vm->reg[MBR];
        }
    }
    else if (tipoOp1 == 2){ //directo
        if (tipoOp2 == 0){ //registro
            vm->reg[LAR] = (vm->reg[DS] & 0xFFFF0000) | (vm->reg[OP1] & 0x0000FFFF); //cargo LAR con segmento de datos y offset del operando
            vm->reg[MBR] = vm->reg[vm->reg[OP2] & 0x00FFFFFF];
            writeMemory(vm);
        }
        else if (tipoOp2 == 1){ //inmediato
            vm->reg[LAR] = (vm->reg[DS] & 0xFFFF0000) | (vm->reg[OP1] & 0x0000FFFF); //cargo LAR con segmento de datos y offset del operando
            vm->reg[MBR] = (vm->reg[OP2] << 8) >> 8; //extiendo el signo
            writeMemory(vm);
        }
    }
}

