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



int main(int argc, char **argv){
    uint8_t *text;
    uint64_t *data;
    uint64_t *stack;

    const uint16_t  POOLSIZE = 255;
    uint8_t pp, dp, sp;
    uint64_t *reg;  // registers

    const char* filename;


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
                       // 3:   Maximum reccursion
    int8_t debug = 0; // debug mode
    int32_t instruction_count = 0;

    while(exit<0){
        instruction_count++;
        if (pp > POOLSIZE-2){puts("Segfault"); return 1;}
        if (sp > POOLSIZE-2){puts("Stack overflow."); return 1;} // on a atteind le bout

        switch (text[pp]){
            case NOP:  pp++;                                        break;
            case EXIT: exit = 0;                                    break;
            case DEBG: pp++;debug = !debug;                         break;
            case JMP:  pp = text[pp+1];                             break;
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
        if (debug){
            printf("(%d, %u) [%llu, %llu, %llu, %llu]\n", instruction_count, pp, reg[RA], reg[RB], reg[RC], reg[RD]);
        }
        if (instruction_count > 60) exit = 3;
    }

    if (exit == 0){
        if(reg[RA]) printf("%llu\n", reg[RA]);
    }else if (exit == 1){
        printf("Bad register address at %u\n", pp);
    }else if (exit == 2){
        printf("Unknown instruction %#x at %u\n", text[pp], pp);
    }else if (exit == 3){
        printf("Too much reccursion, probably infinite loop.\n");
    }


    //              Clean
    free(text);
    free(data);
    free(stack);
    return 0;
}
