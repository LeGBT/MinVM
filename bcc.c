#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "vm.h"


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
        printf("Need a filename to compile.\n");
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
    printf("%s", src);

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

    uint8_t test[FILESIZE] = { LOAD, RD, 25,
                               LOAD, RA, 0,
                               LOAD, RB, 1,
                               MOV, RC, RA,
                               ADD, RA, RB,
                               MOV, RB, RC,
                               DEC, RD,
                               PUSH, RA,
                               JNZ, 9,
                               RET};
    size = fwrite(test, sizeof(uint8_t), 32, pf);
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
