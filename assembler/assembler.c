#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void stripComment(char *line) {
  char *cutoff = strchr(line, ';');
  if (cutoff != NULL) {
    *cutoff = '\0';
  }
}

// convert r0 to 0, r1 to 1, etc.
// returns -1 if invalid
int regStrToInt(char *regStr) {
  if (regStr[0] == 'r') {
    return atoi(regStr + 1);
  }
  printf("syntax error. assembly failed\n");
  exit(-1); // TODO: handle error for real
}

int findNextRegister() {
  char *regStr;
  regStr = strtok(NULL, " ,");
  if (regStr == NULL) {
    printf("syntax error. assembly failed\n");
    exit(-1); // TODO: handle error for real
  }
  return regStrToInt(regStr);
}

int findNextConstant() {
  char *constantStr = strtok(NULL, " ");
  if (constantStr == NULL) {
    printf("syntax error. assembly failed\n"); // TODO: handle error for real
    exit(-1);
  }
  return atoi(constantStr);
}

void writeWord(FILE *target, unsigned short word) {
  printf("writing: %04X\n", word);
  fwrite(&word, sizeof(word), 1, target);
}

void encodeRType(FILE *target, int opcode) {
  int rd, rm, rn;
  unsigned short word;
  rd = findNextRegister();
  rm = findNextRegister();
  rn = findNextRegister();

  word = (opcode << 12) + (rd << 6) + (rm << 3) + rn;
  writeWord(target, word);
}

void encodeIType(int opcode) {
  int rd, rm, immd;
  rd = findNextRegister();
  rm = findNextRegister();
  immd = findNextConstant();

  (opcode << 12) + (rd << 9) + (rm << 6) + immd;
}

void encodeJType(int opcode) {
  int immd;
}

int main(int argc, char **argv) {
  FILE *src, *target;
  char buffer[1024];
  char *token, *targetName = "a.out";
  int rd, rn, rm, immd;

  if (argc == 1) {
    printf("usage: src.s target\n");
    printf("error: no input files\n");
    return -1;
  }

  if (argc > 2) {
    targetName = argv[2];
  }

  src = fopen(argv[1], "r");
  if (src != NULL) {
    target = fopen(targetName, "wb");
    while(fgets(buffer, sizeof(buffer), src)) {
      stripComment(buffer);
      token = strtok(buffer, " ");

      // if the line is empty, skip it
      if (token == NULL) {
        continue;
      }

      if (!strcmp(token, "add")) {
        encodeRType(target, 0);
      }
      else if (!strcmp(token, "sub")) {
        encodeRType(target, 1);
      }
      else if (!strcmp(token, "mul")) {
        encodeRType(target, 2);
      }
      else if (!strcmp(token, "div")) {
        encodeRType(target, 3);
      }
      else if (!strcmp(token, "set")) {
        unsigned short word;
        rd = findNextRegister();
        immd = findNextConstant();
        word = (5 << 12) + (rd << 9) + immd;
        writeWord(target, word);
      }
      else {
        printf("error: operation \"%s\" does not exist\n", token);
        return 1;
      }
    }
    fclose(src);
    fclose(target);
  }
  else {
    printf("vasm: input file %s does not exist\n", argv[1]);
  }
  return 0;
}

