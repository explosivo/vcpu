#ifndef ERROR_H
#define ERROR_H

enum errorType {
  NO_REGISTER,
  NO_CONSTANT,
  BAD_LABEL
};

void handleError(enum errorType e, unsigned int lineNum);
#endif
