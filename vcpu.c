#include "vcpu.h"
#include <stdlib.h>

void initialize() {
  int i;
  pc = 0x200;

  for (i = 0; i < 8; i ++)
    r[i] = 0;
}

void load(FILE *fp) {
  for(unsigned char *m = memory + 0x200; !feof(fp); m ++) {
    *m = fgetc(fp);
  }
}

short conv12SignTo16Sign(short num) {
  if (!(num & (1 << 11))) {
    return num;
  }
  return 0xffff ^ 0xfff | num;
}

 int isImmediate(short word) {
   if (word & (1 << 11)) {
     return 1;
   }
   return 0;
 }

void cycle() {
  int i;
  short offset;
  for (;;) {
    for (i = 0; i < 8; i ++)
      printf("r%d = %d ", i, r[i]);
    printf("pc = %d\n", pc);
    //getchar(); TODO: add a runtime argument to choose step through
    unsigned short opcode = memory[pc] << 8 | memory[pc + 1];
    switch(opcode & 0xf000) {
      case 0x0000: // add
        if (!isImmediate(opcode)) {
          r[(opcode & 0700) >> 6] = r[(opcode & 070) >> 3] + r[opcode & 07];
          pc += 2;
        }
        else {
          r[(opcode & 0700) >> 6] = r[(opcode & 070) >> 3] + (memory[pc + 2] << 8 | memory[pc + 3]);
          pc += 4;
        }
        break;
      case 0x1000: // sub
        if (!isImmediate(opcode)) {
          r[(opcode & 0700) >> 6] = r[(opcode & 070) >> 3] - r[opcode & 07];
          pc += 2;
        }
        else {
          r[(opcode & 0700) >> 6] = r[(opcode & 070) >> 3] - (memory[pc + 2] << 8 | memory[pc + 3]);
          pc += 4;
        }
        break;
      case 0x2000: // mul
        if (!isImmediate(opcode)) {
          r[(opcode & 0700) >> 6] = r[(opcode & 070) >> 3] * r[opcode & 07];
          pc += 2;
        }
        else {
          r[(opcode & 0700) >> 6] = r[(opcode & 070) >> 3] * (memory[pc + 2] << 8 | memory[pc + 3]);
          pc += 4;
        }
        break;
      case 0x3000: // div
        if (!isImmediate(opcode)) {
          if (r[opcode & 07] != 0) {
            r[(opcode & 0700) >> 6] = r[opcode & 070 >> 3] / r[opcode & 07];
          }
          pc += 2;
        }
        else {
          if ((memory[pc + 2] << 8 | memory[pc + 3]) != 0) {
            r[(opcode & 0700) >> 6] = r[(opcode & 070) >> 3] / (memory[pc + 2] << 8 | memory[pc + 3]);
          }
          pc += 4;
        }
        break;
      case 0x4000: // jmp
        offset = conv12SignTo16Sign(0x0fff & opcode);
        pc += (offset + 1) * 2;
        break;
      case 0x5000: // set
        r[(opcode & 0700) >> 6] = memory[pc + 2] << 8 | memory[pc + 3];
        pc += 4;
        break;
      case 0x6000: // mov
        r[(opcode & 0700) >> 6] = r[(opcode & 070) >> 3];
        pc += 2;
        break;
      case 0x7000: // bne
        if (r[(opcode & 0700) >> 6] != r[(opcode & 070) >> 3]) {
          pc += 2;
        }
        else {
          pc += 4;
        }
        break;
      case 0x8000: // beq
        if (r[(opcode & 0700) >> 6] == r[(opcode & 070) >> 3]) {
          pc += 2;
        }
        else {
          pc += 4;
        }
        break;
      case 0x9000: // blt
        if (r[(opcode & 0700) >> 6] < r[(opcode & 070) >> 3]) {
          pc += 2;
        }
        else {
          pc += 4;
        }
        break;
      case 0xa000: // bgt
        if (r[(opcode & 0700) >> 6] > r[(opcode & 070) >> 3]) {
          pc += 2;
        }
        else {
          pc += 4;
        }
        break;
      case 0xb000: // ble
        if (r[(opcode & 0700) >> 6] <= r[(opcode & 070) >> 3]) {
          pc += 2;
        }
        else {
          pc += 4;
        }
        break;
      case 0xc000: // bge
        if (r[(opcode & 0700) >> 6] >= r[(opcode & 070) >> 3]) {
          pc += 2;
        }
        else {
          pc += 4;
        }
        break;
      case 0xf000:
        printf("terminating\n");
        return;
      default:
        printf("error: instruction %04x does not exist\n", opcode & 0xff00);
        pc += 2;
        break;
    }
  }
}
