#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "vm.h"

#define rx  reg[text[pp]%4]
#define rxn reg[text[pp+1]%4]

/*
 *  +------------------+
 *  |    stack         |      high address
 *  +------------------+
 *  | data segment     |
 *  +------------------+
 *  | text segment     |      low address
 *  +------------------+
 */

uint8_t *text;
uint64_t *data;
uint64_t *stack;

const uint16_t  POOLSIZE = 255;
uint8_t pp, dp, sp;
uint64_t *reg;  // registers

const char* filename;
char* src;


int main(int argc, char **argv){



    //              Allocating VM mem
    text = malloc(POOLSIZE*sizeof(uint8_t));
    data = calloc(POOLSIZE, sizeof(uint64_t));
    stack = calloc(POOLSIZE, sizeof(uint64_t));
    reg = calloc(4, sizeof(uint64_t));

    if(!(text && data && stack && reg)){
        printf("Could not allocate VM memory.\n");
        return 1;
    }



    //                Reading bytecode
    if (argc != 2){
        printf("Need a filename to compile.\n");
        return 1;
    }

    argc--;
    argv++;
    filename = argv[0];

    FILE *fp = fopen(filename, "r");
    if (!fp){
        printf("Can't open file %s for reading.\n", filename);
        return 1;
    }

    size_t r = fread(text, sizeof(uint8_t), POOLSIZE-4, fp);
    fclose(fp);
    if(r == POOLSIZE-4){
        printf("***ERROR MAXIMUM FILESIZE***\n");
        free(src);
        return 1;
    }



    //                  Start VM
    sp = 0;
    dp = 0;
    pp = 0;

    int8_t exit = -1; // exit code:
                       // 0:   OK
                       // 1:   Bad register address
                       // 2:   Unknown instruction
                       // 100: Debug
    while(exit<0){
        if (pp > POOLSIZE-2){puts("Segfault"); return 1;}
        if (sp > POOLSIZE-2){puts("Stack overflow."); return 1;} // on a atteind le bout

        switch (text[pp]){
            case NOP:  pp++;                                        break;
            case EXIT: exit = 0;                                    break;
            case DEBG: exit = 100;                                  break;
            case JMP:  pp++; pp = rx;                               break;
            case JZ:   if(reg[RD]==0){pp=text[pp+1];}else{pp+=2;}   break;
            case JNZ:  if(reg[RD]!=0){pp=text[pp+1];}else{pp+=2;}   break;
            case PUSH: pp++; sp++; stack[sp] = rx; pp++;            break;
            case POP:  pp++; rx = stack[sp];sp--; pp++;             break;
            case LOAD: pp++; rx = text[pp+1]; pp+=2;                break;
            case MOV:  pp++; rx = rxn; pp+=2;                       break;
            case INC:  pp++; rx++; pp++;                            break;
            case DEC:  pp++; rx--; pp++;                            break;

            // case CALL: break; //TODO not implemented yet
            case PUTC: pp++; putchar((char)rx); pp++;               break;

            case ADD:  pp++; rx += rxn; pp+=2;                      break;
            case SUB:  pp++; rx -= rxn; pp+=2;                      break;
            case MUL:  pp++; rx *= rxn; pp+=2;                      break;
            default: exit = 2;
        }

    }

    if (exit % 100 == 0){
        for(;sp>0;sp--){
            printf("%llu\n", stack[sp]);
        }
        if (exit == 100){
            printf("%llu\t%llu\t%llu\t%llu\n", reg[RA], reg[RB],
                    reg[RC], reg[RD]);
        }
    }else if (exit == 1){
        printf("Bad register address at %u\n", pp);
    }else if (exit == 2){
        printf("Unknown instruction %u\n", text[pp]);
    }


    //              Clean
    free(src);
    free(text);
    free(data);
    free(stack);
    return 0;
}
