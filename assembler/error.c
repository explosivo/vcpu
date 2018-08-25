#include <error.h>
#include <stdio.h>
#include <stdlib.h>

void handleError(enum errorType e, unsigned int lineNum) {
  printf("error: line %d\n");
  switch(e) {
    case NO_REGISTER:
      printf("expected a valid register\n");
      exit(e);
    case NO_CONSTANT:
      printf("expected a valid constant\n");
      exit(e);
    case BAD_LABEL:
      printf("expected a valid label\n");
      exit(e);
    default:
      printf("unexpected error.\n");
      exit(-1);
  }
}
