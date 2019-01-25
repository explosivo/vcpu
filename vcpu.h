#ifndef VCPU_H
#define VCPU_H

unsigned char memory[4096];
unsigned short r[8];
unsigned short pc;

void initialize();
void load(FILE *fp);
void cycle();

#endif
