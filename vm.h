#define EXIT    0x11  // EXIT;          stop programm
#define JMP     0x12  // JMP RA;        jump to label pointed by RA
#define JZ      0x13  // JMPZ 5;        jump to label 5 if RD == 0
#define JNZ     0x14  // JMPNZ 5;       jump to label 5 if RD != 0
#define LABL    0x15  // LABL;          label

#define PUSH    0x20  // PUSH RA;       push RA to stack
#define POP     0x21  // POP RA;        pop stack to RA
#define LOAD    0x22  // LOAD RA 5;     RA = 5
#define MOV     0x23  // MOV RA RB;     RA = RB
#define INC     0x24  // INC RA;        increment RA
#define DEC     0x25  // DEC RA;        decrement RA

#define CALL    0x30  //                call function at ax
#define PUTC    0x31  // PUTC RA;       print RA as char
#define DEBG    0x32  // DEBUG;         stop programm and display registers

#define ADD     0x40  // ADD RA RB      RA = RA + RB
#define SUB     0x41  // SUB RA RB      RA = RA - RB
#define MUL     0x42  // MUL RA RB      RA = RA * RB

#define NOP     0x80  // NOP;           do nothing
#define RA  0x00
#define RB  0x01
#define RC  0x02
#define RD  0x03

