#define NOP     0x00  // NOP;           do nothing
#define EXIT    0x01  // EXIT;          stop programm
#define JMP     0x02  // JMP RA;        jump to instruction pointed by RA
#define JZ      0x03  // JMPZ 5;        jump to 5 if RD == 0
#define JNZ     0x04  // JMPNZ 5;       jump to 5 if RD != 0

#define PUSH    0x10  // PUSH RA;       push RA to stack
#define POP     0x11  // POP RA;        pop stack to RA
#define LOAD    0x12  // LOAD RA 5;     RA = 5
#define MOV     0x13  // MOV RA RB;     RA = RB
#define INC     0x14  // INC RA;        increment RA
#define DEC     0x15  // DEC RA;        decrement RA

#define CALL    0x30  //                call function at ax
#define PUTC    0x31  // PUTC RA;       print RA as char
#define DEBG    0x32  // DEBUG;         stop programm and display registers

#define ADD     0x50  // ADD RA RB      RA = RA + RB
#define SUB     0x51  // SUB RA RB      RA = RA - RB
#define MUL     0x52  // MUL RA RB      RA = RA * RB

#define RA  0x00
#define RB  0x01
#define RC  0x02
#define RD  0x03

