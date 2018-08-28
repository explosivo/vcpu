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

/*
 * TODO - redesign isa.
 * there are only 7 registers... we are wasting one bit everytime
 * a register is represented. they do not require four bits, only
 * three.
 *
 * four bits for the instruction, three bits for flags, three bits
 * each time a register needs to be represented.
 *
 * op   flg Rd  Rn  Rm
 * 0000 000 000 000 000
 *
 * it should be possible to store 16 bit numbers in a register.
 * currently it is only possible to store 8 bit numbers.
 * set a flag on the instruction, the next instruction will
 * represent the number. there will be no need for a set instruction.
 *
 * i.e.
 * source:
 *   mov r3, #65535
 * binary:
 *   0110100011000000
 *   1111111111111111
 *
 * source:
 *   mov r3, r2
 * binary:
 *   0110000011010000
 *
 * ...why am i working on this?
 */
void cycle() {
  int i;
  short offset;
  for (;;) {
    for (i = 0; i < 8; i ++)
      printf("r%d = %d ", i, r[i]);
    printf("\n");
    unsigned short opcode = memory[pc] << 8 | memory[pc + 1];
    switch(opcode & 0xf000) {
      case 0x0000: // add
        r[(opcode & 0700) >> 6] = r[(opcode & 070) >> 3] + r[opcode & 07];
        pc += 2;
        break;
      case 0x1000: // sub
        r[(opcode & 0700) >> 6] = r[(opcode & 070) >> 3] - r[opcode & 07];
        pc += 2;
        break;
      case 0x2000: // mul
        r[(opcode & 0700) >> 6] = r[(opcode & 070) >> 3] * r[opcode & 07];
        pc += 2;
        break;
      case 0x3000: // div
        if (r[opcode & 07] != 0) {
          r[(opcode & 0700) >> 6] = r[opcode & 070 >> 3] / r[opcode & 07];
        }
        pc += 2;
        break;
      case 0x4000: // jmp
        offset = conv12SignTo16Sign(0x0fff & opcode);
        pc += (offset + 1) * 2;
        break;
      case 0x5000: // set
        r[(opcode & 07000) >> 9] = memory[pc + 2] << 8 | memory[pc + 3];
        pc += 4;
        break;
      case 0x6000: // mov
        r[(opcode & 0700) >> 6] = r[(opcode & 070) >> 3];
        pc += 2;
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
