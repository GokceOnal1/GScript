#include "AST.h"

/*Constructs an AST node with type TYPE (sets initial values to NULL)*/
AST* ast_init(int type) {
	AST* ast = calloc(1, sizeof(AST));
	
	ast->type = type;
	ast->scope = (void*)0;
	ast->var_def_var_name = (void*)0;
	ast->var_def_value = (void*)0;
	ast->func_def_body = (void*)0;
	ast->func_def_name = (void*)0;
	ast->func_def_args = (void*)0;
	ast->func_def_args_size = 0;
	ast->var_name = (void*)0;
	ast->func_call_name = (void*)0;
	ast->func_call_args = (void*)0;
	ast->func_call_args_size = 0;
	ast->str_value = (void*)0;
	ast->compound_value = (void*)0;
	ast->compound_size = 0;
  ast->line = 0;
  ast->num_value = 0;
  ast->left = (void*)0;
  ast->op = 0;
  ast->right = (void*)0;
  ast->b_value = 0;
	
	return ast;
}