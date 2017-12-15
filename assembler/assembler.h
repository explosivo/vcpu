#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void initialize();
void removeWhitespace(char **s);
int parse(char *line);
int set(char **arguments);
int checkRegister(char* s);
int isNumber(char c);