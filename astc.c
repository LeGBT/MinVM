#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


#define push(x) p++; stack[p]=x
#define pop() stack[p]; p--

#define MAXSTACK 256


const char *astest = "(define (carre x) (* x x))\n"
                     "(define t (* 10 (+ 3 4)))\n"
                     "(carre t)\n";

typedef enum Symbol_enum Symbol;


enum Symbol_enum {
    start,
    expr,
    op,
    num,
    test,

    ENUM_COUNT,
};



void next_symbol(Symbol *stack, uint8_t *p,  char c){
    (*p)++;
    stack[*p] = c;
    printf("stack:%c\n", stack[*p]);
}



int main(int argc, char **argv){

    Symbol stack[MAXSTACK+1] = {0};
    uint8_t p = 0;

    printf("p=%u\n", p);
    printf("Il y a %u enums.\n", ENUM_COUNT);
    next_symbol(stack, &p, astest[p]);
    printf("p=%u\n", p);

    return 0;
}

