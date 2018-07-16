#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

static FILE *filepointer;
static uint64_t line = 1;
static const char* filename;
static uint8_t malloc_count = 0;


enum token_type{
    null_token, lpar_token, rpar_token, string_token, id_token,
    num_token, eof_token, newline_token,
    TOKEN_TYPE_COUNT,
};

enum terminal_type{
    nil, num, id, def, eval,
    TERMINAL_TYPE_COUNT,
};


typedef struct token Token;
struct token{
    union{
        char *s;
        uint64_t u;
    };
    enum token_type type;
    uint32_t padding;
};


typedef struct list_ List;
struct list_{
    union {
        uint64_t u;
        char *s;
    };
    List *tail;
    enum terminal_type head;
    uint8_t paddind[4];
};


char next_char(void);
char peek_char(void);
void eat_spaces(char c);
Token next_token(void);
List* S(uint8_t *stop);
List* E(uint8_t *stop);
List* F(uint8_t *stop);
List* G(uint8_t *stop, Token t);
void clean_list(List* l);


char next_char(){ return (char)fgetc(filepointer); }
char peek_char(){ char c = (char)fgetc(filepointer); ungetc(c, filepointer); return c;}


void eat_spaces(char c){
    while(c == ' ' || c == '\n'){
        c = next_char();
    }
}

Token next_token(){
    Token t;
    t.type = null_token;
    char c = next_char();
    while (c==' ') c = next_char(); // eat all tha spaces bitches
    if (c == '(') {t.type = lpar_token; return t;}
    if (c == ')') {t.type = rpar_token; return t;}
    if (c == EOF) {t.type = eof_token; return t;}
    if (c == '\n') {t.type = newline_token; line++; return t;}
    if (c >= '0' && c<= '9'){
        t.type = num_token;
        t.u = (uint64_t)(c - '0');
        while (peek_char()>='0' && peek_char()<='9'){
            c = next_char();
            t.u *= 10;
            t.u += (uint64_t)(c - '0');
        }
        char n = peek_char();
        if (n!=')' && n!=' ' && n!=']' && n!='\n'){
            printf("Error while parsing line %llu,"
                    "numerals can't end with %c\n", line, n);
        }
        return t;
    }
    if (c == '"'){
        uint8_t size = 0;
        char *mstring = malloc(65*sizeof(char)); // max of 64 char string
        malloc_count++;
        c = next_char();
        while (size<64 && c!='"' && c>=' ' && c<'~' && c!='\n'){
            mstring[size] = c;
            c = next_char();
            size++;
        }
        if (size == 64){
            printf("Error, strings can't be more than 64 char long.(line %llu)\n", line);
            while (c!='"' && c!='\n' && c!=EOF) c = next_char();
        }
        if (c!='"'){
            printf("Missing matching \" in line=%llu\n", line);
        }
        //TODO free nstring when done with the token
        char *nstring = malloc(size*sizeof(char));
        malloc_count++;
        for (uint8_t p=0; p<size; p++) nstring[p] = mstring[p];
        free(mstring);
        malloc_count--;
        nstring[size] = 0;
        t.type = string_token;
        t.s = nstring;
        return t;
    }

    // Everything was parsed but id.
    char *mstring = malloc(33*sizeof(char)); // max id size is 32 char

    malloc_count++;
    uint32_t size = 0;
    while (c!=' ' && c!='(' && c!='"' && c!='[' && c!=']' && c!=')' && c!='\n' &&c!=EOF && size<32){
        if(c<' ' || c>'~'){
            printf("Illegal character {%c} at line %llu\n", c, line);
        }else{
            mstring[size] = c;
        }
        size++;
        c = next_char();
    }
    if (size==32){
        printf("id can't be longer than 32 char at line %llu\n", line);
    }

    char *nstring = malloc((size+1)*sizeof(char));
    malloc_count++;
    for (uint8_t p=0; p<size; p++) nstring[p] = mstring[p];
    nstring[size] = 0;
    free(mstring);
    malloc_count--;
    t.type = id_token;
    t.s = nstring;
    return t;
}


List* S(uint8_t *stop){
    Token t = next_token();
    if (t.type == eof_token){
        *stop = 1; return NULL;
    }
    if (t.type == newline_token){
        List *l = malloc(sizeof(List));
        l->head = nil;
        return l;
    }
    if (t.type == id_token){ // assignement
        // l = ( def ...)
        List *l = malloc(sizeof(List));
        List *ll = malloc(sizeof(List));
        malloc_count += 2;
        l->head = def;
        l->tail = ll;
        ll->head = id;
        ll->s = t.s;
        ll->tail = E(stop);
        return l;
    }
    if (t.type == lpar_token){ // token is consumed
        return F(stop);
    }
    printf("Error at line %llu, unexpected token %u\n", line, t.type);
    *stop = 1;
    return NULL;
}

List *E(uint8_t *stop){
    Token lpar = next_token(); // on consume (
    if (lpar.type != lpar_token){
        printf("Error at line %llu, left parenthesis is missing.\n", line);
        *stop = 1;
        return NULL;
    }
    return F(stop);
}

List *F(uint8_t *stop){
    Token t = next_token();
    if (t.type == id_token){
        if (t.s[0] == 'd' && t.s[1] == 'e' && t.s[2] == 'f' && t.s[3] == 0){
            // def string not needed anymore, freeing
            free(t.s);
            malloc_count--;
            List *l = malloc(sizeof(List));
            malloc_count++;
            l->head = def;
            l->tail = G(stop, next_token());
            return l;
        }
        if (t.s[0]=='e'&&t.s[1]=='v'&&t.s[2]=='a'&&t.s[3]=='l'&&t.s[4]==0){
            // eval string not needed anymore, freeing
            free(t.s);
            malloc_count--;
            List *l = malloc(sizeof(List));
            malloc_count++;
            l->head = eval;
            l->tail = G(stop, next_token());
            return l;
        }
    }
    return G(stop, t); //il ne faut pas consommer le token, on relaie
}

List *G(uint8_t *stop, Token t){
    if (t.type == eof_token || t.type == newline_token){
        printf("Unexpected newline or eof at line %llu\n", line);
        *stop = 1;
        return NULL;
    }
    if (t.type == rpar_token){

        Token tt = next_token();
        if (tt.type != newline_token){
            printf("Error, A new line is expected after ) at line %llu\n", line);
            *stop = 1;
            return NULL;
        }
        List *l = malloc(sizeof(List));
        malloc_count++;
        l->head = nil;
        return l;
    }
    if (t.type == id_token){
        List *l = malloc(sizeof(List));
        malloc_count++;
        l->head = id;
        l->s = t.s;
        l->tail = G(stop, next_token());
        return l;
    }
    if (t.type == num_token){
        List *l = malloc(sizeof(List));
        malloc_count++;
        l->head = num;
        l->u = t.u;
        l->tail = G(stop, next_token());
        return l;
    }
    printf("Unexpected token at line %llu\n", line);
    *stop = 1;
    return NULL;
}

void clean_list(List *l){
    if (l->head == nil){
        free(l);
        malloc_count--;
    }
    if (l->head == id){
        free(l->s);
        malloc_count--;
        clean_list(l->tail);
        free(l);
        malloc_count--;
    }
    if (l->head == num || l->head == def || l->head == eval){
        clean_list(l->tail);
        free(l);
        malloc_count--;
    }
}

int main(int argc, char **argv){
    {
        if (argc != 2){
            printf("Need a single filename to compile.\n");
            return 1;
        }

        argc--;
        argv++;
        filename = argv[0];
        uint8_t filename_size = 0;
        for(;filename[filename_size];filename_size++);

        // check for .li extension
        if (filename[filename_size-1]!='i' || filename[filename_size-2]!='l' || filename[filename_size-3]!='.'){
            printf("Need a .li file.\n");
            return 1;
        }
        filepointer = fopen(filename, "r");

        // check for errors
        if (!filepointer){
            puts("***");
            printf("Can't open file %s for reading.\n", filename);
            puts("***");
            return 1;
        }
    }

    uint8_t *stop = malloc(sizeof(uint8_t));
    malloc_count++;
    *stop = 0;
    List *l = S(stop);
    List *h = l;

    printf("unfreed=%u\n", malloc_count);
    while(*stop==0){
        printf("(");
        while(l->head != nil){
            if (l->head == num){
                printf("%llu ", l->u);
            }else if (l->head == id){
                printf("%s ", l->s);
            }else if (l->head == def){
                printf("def ");
            }else if (l->head == eval){
                printf("eval ");
            }else{
                printf("bad token %u \n", l->head);
            }
            l = l->tail;
        }
        printf(")\n");
        clean_list(h);
        l = S(stop);
    }
    free(stop);
    malloc_count--;
    printf("unfreed=%u\n", malloc_count);

    fclose(filepointer);
    return 0;
}
