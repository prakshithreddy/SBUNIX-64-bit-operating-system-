#ifndef _IDT_H
#define _IDT_H

#include<sys/defs.h>



void init_idt();

uint64_t switchRsp;


uint64_t switchRax;
uint64_t switchRbx;
uint64_t switchRcx;
uint64_t switchRdx;
uint64_t switchRsi;
uint64_t switchRdi;

uint64_t switchRbp;

uint64_t errorCode;

void handleBackspace();
char readChar();
void writeChar(char c,int specialCharacter);
void set_input_buf(uint64_t page);
char * get_input_buf();
#endif
