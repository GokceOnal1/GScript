#include "debug.h"

void printast(unsigned type) {
  if(type>17) exit(1);
  char* types [17] = {"AST_VAR_DEF", "AST_FUNC_DEF", "AST_VAR", "AST_FUNC_CALL", "AST_STR", "AST_COMPOUND", "AST_NOOP", "AST_NUM", "AST_BINOP", "AST_BOOL", "AST_IF", "AST_REPEAT", "AST_OUT", "AST_SKIP", "AST_REASSIGN", "AST_WHILE", "AST_UNOP"};
  printf("%s\n", types[type]);
}