#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "vm2.h"

int main(int argc, char **argv){
    const uint16_t STACKSIZE = 65535;
    const uint16_t TEXTSIZE = 65535;
    const uint8_t  DATASIZE  = 255;

    uint64_t *stack = malloc(STACKSIZE*8);
    uint8_t *text = malloc(TEXTSIZE);
    uint64_t *data = malloc(DATASIZE*8);
    uint16_t tpx = 0; // text pointer register
    uint16_t sp = 0;  // stack pointer

    if(!(text && data && stack)){
        printf("Could not allocate VM memory.\n");
        return 1;
    }

    if (argc != 2){
        printf("Need a filename to compile.\n");
        return 1;
    }

    argc--;
    argv++;
    const char *filename = argv[0];

    FILE *fp = fopen(filename, "r");
    if (!fp){
        printf("Can't open file %s for reading.\n", filename);
        return 1;
    }

    size_t r = fread(text, 1, TEXTSIZE-8, fp);
    fclose(fp);
    if(r == TEXTSIZE-8){
        printf("***ERROR MAXIMUM FILESIZE***\n");
        return 1;
    }


    //                  Start VM


    // exit code:
    //      0=ok
    //      1=segfault
    //      2=stack overflow
    //      3=stack underflow
    //      4=unknown address
    //      5=invalid test value
    //      6=invalid char
    //      7=data overflow
    int8_t exit = -1;

    while(exit<0){
        if (tpx > TEXTSIZE-8){
            printf("Out of the program ! (maybe missed EXIT)\n");
            break;
        }
        if (sp > STACKSIZE-1){
            printf("Stack overflow.");
            break;
        }
        switch (text[tpx]){
            // NOP
            case NOP: tpx++; break;

            // EXIT
            case EXIT: exit = 0; break;

            // JUMP addr1-addr0
            case JUMP:
                   {
                    uint16_t addr = text[tpx+1];
                    addr <<= 8;
                    addr += text[tpx+2];
                    tpx = addr;
                    break;
                   }

            // JIF addr1-addr0 (stack(test))
            case JIF:
                    if (sp < 1){ exit = 3; break;}
                    if (stack[sp]>1){ exit = 5; break;}
                    if (stack[sp]){
                        uint16_t addr = text[tpx+1];
                        addr <<= 8;
                        addr += text[tpx+2];
                        tpx = addr;
                    }else{
                        tpx += 3;
                    }
                    sp -= 1;
                    break;

            // NOT
            case NOT:
                    if (stack[sp]>1){ exit = 5; break;}
                    stack[sp] = 1 - stack[sp];
                    tpx++;
                    break;

            // PUSHB w1
            case PUSHB:
                if (sp == STACKSIZE-1){ exit = 2; break;}
                sp++;
                stack[sp] = (uint64_t)text[tpx+1];
                tpx += 2;
                break;

            // PUSHW w1 w2
            case PUSHW:
                if (sp == STACKSIZE-1){ exit = 2; break;}
                sp++;
                stack[sp] = ((uint64_t)text[tpx+1]<<8) + ((uint64_t)text[tpx+2]<<0);
                tpx += 3;
                break;

            // PUSHD w1 w2 w3 w4
            case PUSHD:
                if (sp == STACKSIZE-1){ exit = 2; break;}
                sp++;
                stack[sp]=((uint64_t)text[tpx+1]<<24) + ((uint64_t)text[tpx+2]<<16) +
                          ((uint64_t)text[tpx+3]<< 8) + ((uint64_t)text[tpx+4]<<0);
                tpx += 5;
                break;

            // PUSHQ w1 w2 w3 w4 w5 w6 w7 w8
            case PUSHQ:
                if (sp == STACKSIZE-1){ exit = 2; break;}
                sp++;
                stack[sp]=((uint64_t)text[tpx+1]<<56) + ((uint64_t)text[tpx+2]<<48) +
                          ((uint64_t)text[tpx+3]<<40) + ((uint64_t)text[tpx+4]<<32) +
                          ((uint64_t)text[tpx+5]<<24) + ((uint64_t)text[tpx+6]<<16) +
                          ((uint64_t)text[tpx+7]<< 8) + ((uint64_t)text[tpx+8]<<0);
                tpx += 9;
                break;

            // STORE addr
            case STORE:
                if (text[tpx+1] > DATASIZE-1){ exit = 7; break;}
                data[text[tpx+1]] = stack[sp];
                tpx += 2;
                break;

            // LOAD addr
            case LOAD:
                if (sp == STACKSIZE-1){ exit = 2; break;}
                sp++;
                stack[sp] = data[text[tpx+1]];
                tpx += 2;
                break;

            // POP
            case POP:
                if (sp < 1){ exit = 3; break;}
                sp--;
                tpx++;
                break;

            // DUP
            case DUP:
                if (sp == STACKSIZE-1){ exit = 2; break;}
                if (sp==0){ exit = 3; break;}
                stack[sp+1] = stack[sp];
                sp++;
                tpx++;
                break;

            // INC
            case INC:
                    if (sp < 1){ exit = 3; break;}
                    stack[sp]++; tpx++; break;

            // DEC
            case DEC:
                    if (sp < 1) exit = 3;
                    stack[sp]--; tpx++; break;

            // PUTC
            case PUTC:
                     if (sp < 1) exit = 3;
                     if (stack[sp] > 255) exit = 6;
                     putchar((uint8_t)stack[sp]);
                     tpx++;
                     break;

            // ADD
            case ADD:
                    if (sp < 2){ exit = 3; break;}
                    stack[sp-1] += stack[sp];
                    sp--;
                    tpx++;
                    break;

            // SUB
            case SUB:
                    if (sp < 2){ exit = 3; break;}
                    stack[sp-1] -= stack[sp];
                    sp--;
                    tpx++;
                    break;

            // MUL
            case MUL:
                    if (sp < 2){ exit = 3; break;}
                    stack[sp-1] *= stack[sp];
                    sp--;
                    tpx++;
                    break;

            // EQ
            case EQ:
                    if (sp < 2){ exit = 3; break;}
                    stack[sp-1] = (stack[sp-1]==stack[sp]);
                    sp--;
                    tpx++;
                    break;

            default: exit = 1; break;
        }
    }
    if (exit != 0){
        printf("Error %u\n", exit);
    }else{
        printf("%llu\n", stack[sp]);
    }


    free(data);
    free(text);
    free(stack);
}
