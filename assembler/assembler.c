#include "assembler.h"



void initialize()
{
    
}

void removeWhitespace(char **s)
{
    int i, j = 0;
    printf("this far\n");
    for (i = 0; i < strlen(*s); i ++)
    {
        printf("%c\n", *s[i]);
        if (*s[i] != ' ' || *s[i] != '\n')
        {
            s[0][0] = 'a';
            printf("eh?\n");
            j ++;
        }
    }
    printf("before null\n");
    *s[j] = '\0';
    printf("before return\n");
}

int parse(char *line)
{
    int i, j, numOfArguments = 0;
    int success = 0;
    unsigned short opcode, temp;
    char *instruction, buffer[32], *arguments[4], *test;
    printf("parsing: %s\n", line);
    if (line[0] == ';')
    {
        return;
    }
    for (i = 0; i < strlen(line); i ++)
    {
        if (line[i] == ' ')
        {
            instruction = malloc(sizeof(char) * i);
            for (j = 0; j <= i; j ++)
            {
                if (j < i)
                {
                    instruction[j] = line[j];
                }
                else
                {
                    instruction[j] == '\0';
                }
            }
            break;
        }
    }
    test = malloc(sizeof(char*) * 15);
    test = "this is a test";
    removeWhitespace(&test);
    printf("%s\n", test);
    
    if (strcmp(instruction, "set") == 0)
    {
        //set(arguments);
    }
    free(instruction);
    return success;
}
/*
int set(char **arguments)
{
    opcode = 0x5000;
    for (j = i; j < strlen(line); j ++)
    {
        if (line[j] == ' ')
        {
            i ++;
        }
        else if (line[j] == ',')
        {
            strncpy(argument, line + i, j - i);
            if ((temp = (unsigned short) checkRegister(argument)) == 0xffff)
            {
                return;
            }
            else
            {
                opcode = opcode | temp;
                break;
            }
        }
    }
    for (i = j + 1; i < strlen(line); i ++)
    {
        if (line[i] == ' ')
        {
            j ++;
        }
        else if (isNumber(line[i]))
        {
            for (j = i; j < strlen(line); j ++)
            {
                if ((j - i) > 2)
                {
                    argument[3] = '\0';
                    break;
                }
                else if (isNumber(line[j]))
                {
                    argument[j - i] = line[j];
                }
                else
                {
                    return;
                }
            }
            argument[j - i] = '\0';
            opcode = opcode | (0x00ff & (unsigned short) (atoi(argument)));
            success = 1;
        }
    }
}*/

int checkRegister(char* s)
{
    if (strcmp(s, "r0") == 0)
        return 0x0000;
    if (strcmp(s, "r1") == 0)
        return 0x0100;
    if (strcmp(s, "r2") == 0)
        return 0x0200;
    if (strcmp(s, "r3") == 0)
        return 0x0300;
    if (strcmp(s, "r4") == 0)
        return 0x0400;
    if (strcmp(s, "r5") == 0)
        return 0x0500;
    if (strcmp(s, "r6") == 0)
        return 0x0600;
    if (strcmp(s, "r7") == 0)
        return 0x0700;
    printf("Error: no such register \"%s\"\n", s);
    return 0xffff;
}

int isNumber(char c)
{
    if (c > 47 && c < 58)
        return 1;
    printf("Error: was expecting a number\n");
    return 0;
}