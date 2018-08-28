#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"

// TODO - reorganize this into multiple files

enum opcode {
  ADD,
  SUB,
  MUL,
  DIV,
  JMP,
  SET,
  MOV,
  BNE,
  BEQ,
  BLT,
  BGT,
  BLE,
  BGE,
  LW,
  SW
};

short conv12Signto16Sign(short num) {
  if (num >= 0) {
    return num;
  }
  return 0xffff ^ 0xfff | num;
}

struct Label {
  char *symbol;
  short addr;
  struct Label *next;
};

struct Word {
  unsigned short val;
  struct Word *next;
};

struct JumpAddress {
  struct Word *word;
  short addr;
  char *symbol;
  struct JumpAddress *next;
};

// TODO - rewrite to avoid global variables
// writing dirty, will clear later
unsigned int wordPos = 0;
unsigned int lineNum = 0;
struct Label *labels = NULL;
struct Word *wordsHead = NULL, *wordsTail = NULL;
struct JumpAddress *jumpAddrs = NULL;

void addLabel(char *symbol) {
  struct Label *tmp = malloc(sizeof(struct Label));
  tmp->symbol = malloc(sizeof(char) * strlen(symbol));
  strcpy(tmp->symbol, symbol);
  tmp->addr = wordPos;
  tmp->next = labels;
  labels = tmp;
}

void addJumpAddress(struct Word *word, char *symbol){
  struct JumpAddress *tmp = malloc(sizeof(struct JumpAddress));
  tmp->symbol = malloc(sizeof(char) * strlen(symbol));
  strcpy(tmp->symbol, symbol);
  tmp->word = word;
  tmp->addr = wordPos;
  tmp->next = jumpAddrs;
  jumpAddrs = tmp;
}

// TODO: linear search is garbage. make this faster later
struct Label *getLabel(char *symbol) {
  for (struct Label *label = labels; label != NULL; label = label->next) {
    if (!strcmp(label->symbol, symbol)) {
      return label;
    }
  }
  return NULL;
}

void fillJumpAddresses() {
  for (struct JumpAddress *jump = jumpAddrs; jump != NULL; jump = jump->next) {
    struct Label *label = getLabel(jump->symbol);
    if (label != NULL) {
      short offset = label->addr - jump->addr;
      jump->word->val = jump->word->val | (offset & 0x0fff);
    }
    else {
      handleError(BAD_LABEL, lineNum);
      // TODO: fix error handling
      // assembler is going to give the incorrect line number for these errors
    }
  }
}

void addWord(unsigned short word) {
  struct Word *tmp = malloc(sizeof(struct Word));
  tmp->val = word;
  tmp->next = NULL;
  if (wordsTail == NULL) {
    wordsHead = wordsTail = tmp;
  }
  else {
    wordsTail->next = tmp;
    wordsTail = tmp;
  }
  wordPos ++;
}

void freeLabels(struct Label *label) {
  if (label == NULL) {
    return;
  }
  freeLabels(label->next);
  free(label);
}

void freeJumpAddrs(struct JumpAddress *jump) {
  if (jump == NULL) {
    return;
  }
  freeJumpAddrs(jump->next);
  free(jump);
}

void freeWords(struct Word *word) {
  if (word == NULL) {
    return;
  }
  freeWords(word->next);
  free(word);
}

void cleanup() {
  freeLabels(labels);
  freeWords(wordsHead);
  freeJumpAddrs(jumpAddrs);
}

// convert r0 to 0, r1 to 1, etc.
// returns -1 if invalid
int regStrToInt(char *regStr) {
  if (regStr[0] == 'r') {
    return atoi(regStr + 1);
  }
  handleError(NO_REGISTER, lineNum);
}

int findNextRegister() {
  char *regStr;
  regStr = strtok(NULL, " ,");
  if (regStr == NULL) {
    handleError(NO_REGISTER, lineNum);
  }
  return regStrToInt(regStr);
}

int findNextConstant() {
  char *constantStr = strtok(NULL, " ");
  if (constantStr == NULL) {
    handleError(NO_CONSTANT, lineNum);
  }
  return atoi(constantStr);
}

int validLabel(char *symbol) {
  for (char c = *symbol; c != '\0'; c = *(++symbol)) {
    if (c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' || c >= '0' && c <= '9') {
      continue;
    }
    else {
      return 0;
    }
  }
  return 1;
}

char *findNextSymbol() {
  char* symbol = strtok(NULL, " \n");
  if (symbol != NULL) {
    if (validLabel(symbol)) {
      return symbol;
    }
    else {
      handleError(BAD_LABEL, lineNum);
    }
  }
  else {
    handleError(BAD_LABEL, lineNum);
  }
}

void stripComment(char *line) {
  char *cutoff = strchr(line, ';');
  if (cutoff != NULL) {
    *cutoff = '\0';
  }
}

// this function checks if there is a label in a line.
// if there is a label, it adds it to the labels list
// and returns the position in the line following the
// label.
// if there is no label, it just returns the position
// to the beginning of the line.
// i might refactor this later for better readability.
char *checkForLabel(char *line) {
  char *labelEnd = strchr(line, ':');
  if (labelEnd != NULL) {
    *labelEnd = '\0';
    if (validLabel(line)) {
      addLabel(line);
      return labelEnd + 1;
    }
  }
  return line;
}

void writeWord(FILE *target, unsigned short word) {
  printf("writing: %04X\n", word);
  fputc(word >> 8, target);
  fputc(word & 0x00ff, target);
  wordPos ++;
}

void writeWords(FILE *target) {
  for (struct Word *word = wordsHead; word != NULL; word = word->next) {
    writeWord(target, word->val);
  }
}

void encodeRType(int opcode) {
  int rd, rm, rn;
  int useRn = (opcode != MOV) && (opcode < BNE && opcode > BGE);
  unsigned short word;
  printf("useRn: %d\n", useRn);
  rd = findNextRegister();
  rm = findNextRegister();
  rn = useRn ? findNextRegister() : 0;

  word = (opcode << 12) + (rd << 6) + (rm << 3) + rn;
  addWord(word);
}

void encodeIType(int opcode) {
  int rd, rm;
  int useRm = (opcode != SET);
  unsigned short word, immd;
  rd = findNextRegister();
  rm = useRm ? findNextRegister() : 0;
  immd = findNextConstant();

  word = (opcode << 12) + (1 << 11) + (rd << 6) + (rm << 3);
  addWord(word);
  addWord(immd);
}

void encodeJType(int opcode) {
  unsigned short word;
  char *symbol = findNextSymbol();
  printf("sym: %s\n", symbol);
  word = opcode << 12;
  addWord(word);
  addJumpAddress(wordsTail, symbol);
}



int main(int argc, char **argv) {
  FILE *src, *target;
  char buffer[1024], *line;
  char *token, *targetName = "a.out";
  int rd, rn, rm;
  unsigned short immd;

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
      lineNum ++;
      printf("%s", buffer);
      stripComment(buffer);
      line = checkForLabel(buffer);
      token = strtok(line, " ");

      // if the line is empty, skip it
      if (token == NULL || *token == '\n') {
        continue;
      }

      if (!strcmp(token, "add")) {
        encodeRType(ADD);
      }
      else if (!strcmp(token, "sub")) {
        encodeRType(SUB);
      }
      else if (!strcmp(token, "mul")) {
        encodeRType(MUL);
      }
      else if (!strcmp(token, "div")) {
        encodeRType(DIV);
      }
      else if (!strcmp(token, "addi")) {
        encodeIType(ADD);
      }
      else if (!strcmp(token, "subi")) {
        encodeIType(SUB);
      }
      else if (!strcmp(token, "muli")) {
        encodeIType(MUL);
      }
      else if (!strcmp(token, "divi")) {
        encodeIType(DIV);
      }
      else if (!strcmp(token, "jmp")) {
        encodeJType(JMP);
      }
      else if (!strcmp(token, "set")) {
        encodeIType(SET);
      }
      else if (!strcmp(token, "mov")) {
        encodeRType(MOV);
      }
      else if (!strcmp(token, "bne")) {
        encodeRType(BNE);
        encodeJType(JMP);
      }
      else if (!strcmp(token, "beq")) {
        printf("branch eq\n");
        encodeRType(BEQ);
        //encodeJType(JMP);
      }
      else if (!strcmp(token, "blt")) {
        encodeRType(BLT);
        encodeJType(JMP);
      }
      else if (!strcmp(token, "bgt")) {
        encodeRType(BGT);
        encodeJType(JMP);
      }
      else if (!strcmp(token, "ble")) {
        encodeRType(BLE);
        encodeJType(JMP);
      }
      else if (!strcmp(token, "bge")) {
        encodeRType(BGE);
        encodeJType(JMP);
      }
      else {
        printf("error: operation \"%s\" does not exist\n", token);
        return 1;
      }
    }

    addWord(0xf000);

    fillJumpAddresses();
    writeWords(target);

    fclose(src);
    fclose(target);
    cleanup();
  }
  else {
    printf("vasm: input file %s does not exist\n", argv[1]);
  }
  return 0;
}

