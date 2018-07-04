#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

static FILE *filepointer;
static uint64_t line = 1;
static const char* filename;
static char* src;


enum token_type{
    null, l_par, r_par, l_braket, r_braket, string, id, num, eof, newline,
    TOKEN_TYPE_COUNT,
};

typedef struct token Token;
struct token{
    char *id;
    uint64_t num;
    enum token_type type;
    uint32_t padding;
};

typedef struct Ast AST;
struct Ast{
    uint64_t type;
    AST* right;
    union{
        AST* left;
        Token* token;
    };
};


char next_char(void);
char peek_char(void);
void eat_spaces(char c);
void erase_token(Token t);
Token next_token(void);
void clean_ast(AST *ast);
AST* leaf(Token t);
AST* expr(Token token, Token next_token);
AST* nil(void);






char next_char(){ return (char)fgetc(filepointer); }
char peek_char(){ char c = (char)fgetc(filepointer); ungetc(c, filepointer); return c;}


void eat_spaces(char c){
    while(c == ' ' || c == '\n'){
        c = next_char();
    }
}

void erase_token(Token t){
        switch (t.type){
            case null:
                printf("(null)");
            case l_braket:
                printf("(l_braket)\n");
                break;
            case r_braket:
                printf("(r_braket)\n");
                break;
            case l_par:
                printf("(l_par)\n");
                break;
            case r_par:
                printf("(r_par)\n");
                break;
            case id:
                printf("(id, %s)\n", t.id);
                free(t.id);
                break;
            case num:
                printf("(num, %llu)\n", t.num);
                break;
            case eof:
                printf("(eof)\n");
                break;
            case string:
                printf("(string, %s)\n", t.id);
                free(t.id);
                break;
            case newline:
                printf("(newline, )\n");
                break;
            case TOKEN_TYPE_COUNT:
                printf("(INVALID)");
        }
}


Token next_token(){
    Token t;
    t.type = null;
    char c = next_char();
    while (c==' ') c = next_char(); // eat all tha spaces bitches
    if (c == '(') {t.type = l_par; return t;}
    if (c == ')') {t.type = r_par; return t;}
    if (c == '[') {t.type = l_braket; return t;}
    if (c == ']') {t.type = r_braket; return t;}
    if (c == EOF) {t.type = eof; return t;}
    if (c == '\n') {t.type = newline; line++; return t;}
    if (c >= '0' && c<= '9'){
        t.type = num;
        t.num = (uint64_t)(c - '0');
        while (peek_char()>='0' && peek_char()<='9'){
            c = next_char();
            t.num *= 10;
            t.num += (uint64_t)(c - '0');
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
        for (uint8_t p=0; p<size; p++) nstring[p] = mstring[p];
        free(mstring);
        nstring[size] = 0;
        t.type = string;
        t.id = nstring;
        return t;
    }

    // Everything was parsed but id.
    char *mstring = malloc(33*sizeof(char)); // max id size is 32 char
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
    for (uint8_t p=0; p<size; p++) nstring[p] = mstring[p];
    nstring[size] = 0;
    free(mstring);
    t.type = id;
    t.id = nstring;
    return t;
}

AST* leaf(Token token){
    AST* ast = malloc(sizeof(AST));
    Token* t = malloc(sizeof(Token));
    ast->token = t;
    t->type = token.type;
    // on le garde, faudra le free à la fin avec le reste
    if(token.type==id || token.type==string) t->id = token.id;
    if(token.type == num) t->num = token.num;
    return ast;
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

    next_token(); // (
    Token nt = next_token();
    AST* ast = malloc(sizeof(AST));
    ast->left = leaf(nt);
    printf();



    // while(left!=NULL){
    //     left = left->left;
    // }
    // if(left->token == NULL){
    //     puts("wtf ?");
    // }else{
    //     printf("leftmost=%u", left->token->type);
    // }

    //clean
    free(src);
    return 0;
}
