#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "vm.h"

#define HNOP    10627
#define HEXIT   50638
#define HJMP    7657
#define HJZ     296
#define HJNZ    7694
#define HPUSH   3078
#define HPOP    12085
#define HLOAD   50554
#define HMOV    9904
#define HINC    6942
#define HDEC    3054
#define HCALL   60114
#define HPUTC   3100
#define HDEBG   16902
#define HADD    841
#define HSUB    14420
#define HMUL    10056
#define HRA     487
#define HRB     488
#define HRC     489
#define HRD     490


const uint16_t FILESIZE = 1024;
const char* filename;
char* src;

uint16_t h(char* string){
    uint16_t h = 0;
    uint8_t c = 0;
    uint8_t ch;
    for(c=0;string[c]; c++){
        //string[c] = (string[c] == 32) ? 64 : string[c];
        ch = (string[c] == 32) ? 64 : string[c];
        h *= 27;
        h += ch - 64;
    }
    return h;
}


int main(int argc, char **argv){
    if (argc != 2){
        printf("Need a single filename to compile.\n");
        return 1;
    }

    argc--;
    argv++;
    filename = argv[0];
    FILE *fp = fopen(filename, "r");
    if (!fp){
        puts("***");
        printf("Can't open file %s for reading.\n", filename);
        puts("***");
        return 1;
    }

    // reading...
    if(!(src = calloc(FILESIZE, sizeof(char)))){
        puts("***");
        printf("Could not allocate for source area\n");
        puts("***");
        return 1;
    }

    size_t r = fread(src, sizeof(char), FILESIZE-1, fp);
    if(r == FILESIZE-1){
        puts("***");
        printf("***ERROR MAXIMUM FILESIZE***\n");
        puts("***");
    }
    fclose(fp);
    src[r] = 0; // EOF

    uint8_t size;
    for(size=0;filename[size];size++); // taille de filename
    char out[size+3];
    for(size=0;filename[size];size++){ // on copie filename
        out[size] = filename[size];
    }
    out[size] = '.';                  // on rajoute .bc
    out[size+1] = 'b';
    out[size+2] = 'c';

    FILE *pf = fopen(out, "w");

    uint8_t bytecode[FILESIZE];

    uint64_t c=0;  // char number c
    uint64_t i=0;  // instruction number i
    uint8_t t; // token char
    uint64_t litteral; // litteral 64bit uint
    while(src[c]){
        litteral = 0;
        t=0;
        char token[5] = {0};

        uint8_t isNumber = 0;
        while(src[c]>47 && src[c]<58){
            isNumber++;
            litteral *= 10;
            litteral += src[c]-48;
            c++;
        }
        if (isNumber){
            bytecode[i] = litteral;
            i++;
            //printf("Litteral %llu at %llu\n", litteral, c);
        }
        while(src[c+t] != ' ' && c+t<r && src[c+t] != '\n' && t<4){
            token[t] = (char)src[c+t];
            t++;
        }
        c += t+1;
        switch (h(token)){
            case HNOP:  bytecode[i]=NOP;    i++;break;
            case HEXIT: bytecode[i]=EXIT;   i++;break;
            case HJMP:  bytecode[i]=JMP;    i++;break;
            case HJZ:   bytecode[i]=JZ;     i++;break;
            case HJNZ:  bytecode[i]=JNZ;    i++;break;
            case HPUSH: bytecode[i]=PUSH;   i++;break;
            case HPOP:  bytecode[i]=POP;    i++;break;
            case HLOAD: bytecode[i]=LOAD;   i++;break;
            case HMOV:  bytecode[i]=MOV;    i++;break;
            case HINC:  bytecode[i]=INC;    i++;break;
            case HDEC:  bytecode[i]=DEC;    i++;break;
            case HCALL: bytecode[i]=CALL;   i++;break;
            case HPUTC: bytecode[i]=PUTC;   i++;break;
            case HDEBG: bytecode[i]=DEBG;   i++;break;
            case HADD:  bytecode[i]=ADD;    i++;break;
            case HSUB:  bytecode[i]=SUB;    i++;break;
            case HMUL:  bytecode[i]=MUL;    i++;break;
            case HRA:   bytecode[i]=RA;     i++;break;
            case HRB:   bytecode[i]=RB;     i++;break;
            case HRC:   bytecode[i]=RC;     i++;break;
            case HRD:   bytecode[i]=RD;     i++;break;
            case 0:     ; break;
            default: printf("Instruction (%c%c%c%c) non gérée avec hash(%u)\n", token[0], token[1], token[2], token[3], h(token));
        }
        if (token[4]){printf("error token[4]=%u\n", token[4]);}
    }


    size = fwrite(bytecode, sizeof(uint8_t), 32, pf);
    if (size != 32){
        puts("***");
        printf("***Error while writing to %s***\n", out);
        puts("***");
    }
    fclose(pf);


    //clean
    free(src);
    return 0;
}
