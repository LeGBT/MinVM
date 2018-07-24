#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


typedef struct handle Handle;
typedef struct node Node;

#define MAXSTACK 256


enum type{
    null, expr, fexpr, str, id, num,
    TYPE_COUNT,
};

struct handle{
    const char *err;
    uint32_t p;
    uint32_t max;
    uint32_t perr;
    char cerr;
    uint8_t padding[3];
};

struct node{
    enum type t;
    Node *left;
    Node *right;
};


Handle parse_id (const char *c, Handle h);
Handle parse_num(const char *c, Handle h);
Handle parse_ch (const char *c, Handle h);
Handle parse_T  (const char *c, Handle h);
Handle parse_E  (const char *c, Handle h);
Handle parse_F  (const char *c, Handle h);
Handle parse_G  (const char *c, Handle h);
Handle parse_S  (const char *c, Handle h);


static const char *astest = "(def (head h t) h)\n";

uint8_t str_eq(const char *stringA, const char *stringB, uint8_t count);
Handle eat_spaces(const char *string, Handle h);
Handle parse_id(const char *string, Handle h);
Handle parse_num(const char *string, Handle h);

void next_symbol(Symbol *stack, uint8_t *p,  char c){
    (*p)++;
    stack[*p] = c;
    printf("stack:%c\n", stack[*p]);
}



uint8_t str_eq(const char *stringA, const char *stringB, uint8_t count){
    uint8_t i = 1;
    while(i < count && *stringA && *stringB && *stringA == *stringB){
        stringA++;
        stringB++;
        i++;
    }
    return  (*stringA - *stringB == 0);
}


Handle eat_spaces(const char *string, Handle h){
    if (h.p >= h.max) return h;
    while (string[h.p] == ' ' || string[h.p] == '\n') h.p++;
    return h;
}


Handle parse_id(const char *string, Handle h){
    if (h.p >= h.max || h.err) return h;
    char var[33] = {0};
    uint8_t pos = 0;
    while (string[h.p]!=' ' && string[h.p]!='\n' && string[h.p]!=')' && string[h.p]!=']'){
        //printf("parsing id char with code %u\n", (uint8_t)string[h.p]);
        if (pos>31){
            h.err = "[id] name is too long, maximum is 32 characters.";
            h.perr = h.p;
            h.cerr = string[h.p];
            return h;
        }
        if (string[h.p] < '+' || string[h.p] > 'z' || string[h.p]==':' || string[h.p]=='['){
            h.err = "Illegal character in [id] name.";
            h.perr = h.p;
            h.cerr = string[h.p];
            return h;
        }else{
            var[pos] = string[h.p];
            pos++;
        }
        h.p++;
    }
    h = eat_spaces(string, h);
    return h;
}


Handle parse_num(const char *string, Handle h){
    if (h.p >= h.max || h.err) return h;
    uint64_t num = 0;
    while (string[h.p] != ' ' && string[h.p] != '\n' && string[h.p] != ')' && string[h.p] != ']'){
        num *= 10;
        if (string[h.p] < '0' || string[h.p] > '9'){
            h.err  = "Error while parsing [num], not a digit.";
            h.perr = h.p;
            h.cerr = string[h.p];
            return h;
        }else{
            num += (uint64_t)(string[h.p]-'0');
        }
        h.p++;
    }
    h = eat_spaces(string, h);
    return h;
}


Handle parse_ch(const char *string, Handle h){
    if (h.p >= h.max || h.err) return h;
    if (string[h.p] < ' ' || string[h.p] > '~' || string[h.p]=='"'){
        h.err  = "Error while parsing [char] unvalid value.";
        h.perr = h.p;
        h.cerr = string[h.p];
        return h;
    }
    h.p++;
    return h;
}


Handle parse_T(const char *string, Handle h){
    if (h.p >= h.max || h.err) return h;
    uint32_t start, stop;
    start = h.p;
    if (string[h.p] >= '0' && string[h.p] <= '9'){
        h = parse_num(string, h);
    }else{
        h = parse_id(string, h);
    }
    stop = h.p;
    //printf("[ T.parsed \"%c\"]\t %.*s\n", string[stop], stop-start, string+start);
    return h;
}



Handle parse_E(const char *string, Handle h){
    if (h.p >= h.max || h.err) return h;
    uint32_t start, stop;
    start = h.p;
    if (string[h.p] == '('){
        h.p++;
        h = eat_spaces(string, h);
        h = parse_F(string, h);
        puts("left(");
    }else if(string[h.p] == '['){
        h.p++;
        h = eat_spaces(string, h);
        h = parse_G(string, h);
    }else if(string[h.p] == '"'){
        h.p++;
        h = eat_spaces(string, h);
        h = parse_S(string, h);
        puts("string");
        if (string[h.p] != '"'){
            h.err  = "Missing matching \".";
            h.perr = h.p;
            h.cerr = string[h.p];
            return h;
        }
        h.p++;
        h = eat_spaces(string, h);
    }else if(string[h.p] == ')' || string[h.p] == ']'){
            h.err  = "Too many ) or ].";
            h.perr = h.p;
            h.cerr = string[h.p];
            return h;
    }else{
        h = parse_T(string, h);
        puts("num");
    }
    stop = h.p;
    printf("[ E.parsed \"%c\"]\t %.*s\n", string[stop], stop-start, string+start);
    return h;
}

Handle parse_F(const char *string, Handle h){
    if (h.p >= h.max || h.err) return h;
    uint32_t start, stop;
    start = h.p;
    if(string[h.p]==')'){
        h.p++;
        h = eat_spaces(string, h);
    }else{
        h = parse_E(string, h);
        h = parse_F(string, h);
    }
    stop = h.p;
    //printf("[ F.parsed \"%c\"]\t %.*s \n", string[stop], stop-start, string+start);
    return h;
}

Handle parse_G(const char *string, Handle h){
    if (h.p >= h.max || h.err) return h;
    uint32_t start, stop;
    start = h.p;
    if(string[h.p]==']'){
        h.p++;
        h = eat_spaces(string, h);
    }else{
        h = parse_E(string, h);
        h = parse_G(string, h);
    }
    stop = h.p;
    //printf("[ G.parsed \"%c\"]\t %.*s \n", string[stop], stop-start, string+start);
    return h;
}

Handle parse_S(const char *string, Handle h){
    if (h.p >= h.max || h.err) return h;
    uint32_t start, stop;
    start = h.p;
    if (string[h.p]=='"'){
        // S = ε
    }else{
        h = parse_ch(string, h);
        h = parse_S(string, h);
    }
    stop = h.p;
    //printf("[ S.parsed \"%c\"]\t %.*s\n", string[stop], stop-start, string+start);
    return h;
}


int main(){
    uint8_t max;
    for(max=0;astest[max];max++); // max = strlen(astest)

    Handle h;
    h.p = 0;
    h.max = max;
    h.err = 0;
    h = eat_spaces(astest, h);
    h = parse_E(astest, h);
    if (h.err){
        printf("Error: %s at (%u, %c)\n", h.err, h.perr, h.cerr);
    }
    return 0;
}

