#include "exception.h"
#include <stdio.h>
#include <stdlib.h>

void _GSERR(unsigned line,  char* description) {
  printf(RED "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\ngscript entry: (Line %d):\n" YEL "%s\n", line, description);
  exit(1);
} 