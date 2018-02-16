#include "vcpu.h"
#include <stdio.h>

void initialize()
{
    int i;
    pc = 0x200;
    memory[0x200] = 0x50;
    memory[0x201] = 0x0f;
    memory[0x202] = 0x51;
    memory[0x203] = 0xf0;
    memory[0x204] = 0x00;
    memory[0x205] = 0x01;
    memory[0x206] = 0xff;
    memory[0x207] = 0xff;

    for (i = 0; i < 8; i ++)
        r[i] = 0;
}

void cycle()
{
    int i;
    for (;;)
    {
        for (i = 0; i < 8; i ++)
            printf("r%d = %d ", i, r[i]);
        printf("\n");
        unsigned short opcode = opcode = memory[pc] << 8 | memory[pc + 1];
        switch(opcode & 0xf000)
        {
            case 0x0000: // add
                r[(opcode & 0x00f0) >> 4] += r[opcode & 0x000f];
                pc += 2;
                break;
            case 0x1000: // sub
                r[(opcode & 0x00f0) >> 4] -= r[opcode & 0x000f];
                pc += 2;
                break;
            case 0x2000: // mul
                r[(opcode & 0x00f0) >> 4] *= r[opcode & 0x000f];
                pc += 2;
                break;
            case 0x3000: // div
                r[(opcode & 0x00f0) >> 4] /= r[opcode & 0x000f];
                pc += 2;
                break;
            case 0x4000: // jmp
                pc = opcode & 0x00ff;
                break;
            case 0x5000: // set
                r[(opcode & 0x0f00) >> 8] = opcode & 0x00ff;
                pc += 2;
                break;
            case 0x6000: // mov
                r[(opcode & 0x00f0) >> 4] = r[opcode & 0x000f];
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
