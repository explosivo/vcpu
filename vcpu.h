#include <stdio.h>

unsigned char memory[4096];
unsigned char r[8];
unsigned short pc;

void initialize();
void load(FILE *fp);
void cycle();
