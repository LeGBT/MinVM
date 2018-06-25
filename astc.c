#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


#define push(x) p++; stack[p]=x
#define pop() stack[p]; p--


const char *astest = "(define (carre x) (* x x))\n"
                     "(define t (* 10 (+ 3 4)))\n"
                     "(carre t)\n";

typedef enum Symbol_enum Symbol;

enum Symbol_enum {
    start,
    expr,
    op,
    num,

    ENUM_COUNT,
};




int main(int argc, char **argv){

    Symbol stack[256] = {0};
    uint8_t p = 0;

    push(op);

    for(uint8_t i=0; i<100; i++){
        printf("stack[%u]=%u\n", i, stack[i]);
    }

    return 0;
}
