#include "assembler.h"

int main(int argc, char **argv)
{
    char **lines, buffer[32];
    int numOfLines = 0;
    
    if (argc != 2)
    {
        printf("usage: vasm filename.s\n");
        return 0;
    }
    FILE *fp;
    fp = fopen(argv[1], "r");
    if (fp == NULL)
    {
        printf("vasm: file \"%s\" does not exist\n", argv[1]);
    }
    else
    {
        lines = malloc(sizeof(char**));
        while(fgets(buffer, 32, fp) != NULL)
        {
            ++numOfLines;
            lines = realloc(lines, sizeof(char**) * numOfLines);
            lines[numOfLines - 1] = malloc(sizeof(char*) * 32);
            memcpy(lines[numOfLines - 1], buffer, strlen(buffer));
            parse(lines[numOfLines - 1]);
        }
    }
    
    return 0;
}