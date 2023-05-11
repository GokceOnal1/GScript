#ifndef AST_H
#define AST_H
#include <stdlib.h>
typedef struct AST_s {
  enum { AST_VAR_DEF, AST_FUNC_DEF, AST_VAR, AST_FUNC_CALL, AST_STR, AST_COMPOUND, AST_NOOP, AST_NUM, AST_BINOP, AST_BOOL } type;
	struct scope_s* scope;
  unsigned int line;

	//AST_VAR_DEF
	char* var_def_var_name;
	struct AST_s* var_def_value;

	//AST_FUNC_DEF
	struct AST_s* func_def_body;
	char *func_def_name;
	struct AST_s** func_def_args;
	size_t func_def_args_size;

	//AST_VAR
	char* var_name;

	//AST_FUNC_CALL
	char* func_call_name;
	struct AST_s** func_call_args;
	size_t func_call_args_size;

	//AST_STR
	char* str_value;

	//AST_COMPOUND
	struct AST_s** compound_value;
	size_t compound_size;

  //AST_NUM
  double num_value;

  //AST_BINOP
  struct AST_s* left;
  int op;
  struct AST_s* right;

  //AST_BOOL
  unsigned b_value;
} AST;

AST* ast_init(int type);
#endif