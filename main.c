#include <stdio.h>
#include "vcpu.h"

int main(int argc, char **argv) {
  FILE *fp;

  if (argc == 1) {
    printf("Usage: vcpu [filename]\n");
    return -1;
  }

  fp = fopen(argv[1], "rb");
  if (fp == NULL) {
    printf("file %s does not exist\n", argv[1]);
    return -1;
  }

  initialize();
  load(fp);
  cycle();

  fclose(fp);

  return 0;
}
