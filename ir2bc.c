#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "vm.h"

#define HNOP    10627
#define HEXIT   50638
#define HLABL   40383
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
    uint8_t filename_size = 0;
    for(;filename[filename_size];filename_size++);
    if (filename[filename_size-1]!='r' || filename[filename_size-2]!='i' ||
        filename[filename_size-3]!='.'){
        printf("Need a .ir file.\n");
    }
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

    char* out = calloc(filename_size, sizeof(char));
    uint8_t size;
    for(size=0;size<filename_size-3;size++){ // on copie filename
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
    uint8_t labels[64] = {0};
    uint8_t label_count = 0;
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
            case HLABL: labels[label_count] = i;label_count++;break;
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

    for (uint8_t t=0; t<label_count;t++){
        printf("label(%u)=%u\n", t, labels[t]);
    }

    // label second pass
    for (uint64_t j=0; j<i; j++){
        if (bytecode[j]==JMP || bytecode[j]==JZ || bytecode[j]==JNZ){
            if (bytecode[j+1]>=label_count){
                printf("Unknown label %u at (%llu)\n", bytecode[j+1], j);
            }else{
                bytecode[j+1]=labels[bytecode[j+1]];
                j++;
            }
        }
    }
    printf("%llu instructions\n", i);
    size = fwrite(bytecode, sizeof(uint8_t), i, pf);
    if (size != i){
        puts("***");
        printf("***Error while writing to %s***\n", out);
        puts("***");
    }
    fclose(pf);


    //clean
    free(src);
    free(out);
    return 0;
}
