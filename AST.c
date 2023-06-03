#include "AST.h"

/*Constructs an AST node with type TYPE (sets initial values to NULL)*/
AST *ast_init(int type) {
  AST *ast = calloc(1, sizeof(AST));

  ast->type = type;
  ast->scope = (void *)0;
  ast->var_def_var_name = (void *)0;
  ast->var_def_value = (void *)0;
  ast->func_def_body = (void *)0;
  ast->func_def_name = (void *)0;
  ast->func_def_args = (void *)0;
  ast->func_def_args_size = 0;
  ast->var_name = (void *)0;
  ast->func_call_name = (void *)0;
  ast->func_call_args = (void *)0;
  ast->func_call_args_size = 0;
  ast->str_value = (void *)0;
  ast->compound_value = (void *)0;
  ast->compound_size = 0;
  ast->line = 0;
  ast->num_value = 0;
  ast->left = (void *)0;
  ast->op = 0;
  ast->right = (void *)0;
  ast->b_value = 0;
  ast->if_bodies = (void *)0;
  ast->if_bodies_size = 0;
  ast->if_conds_size = 0;
  ast->if_conds = (void *)0;
  ast->else_body = (void *)0;
  ast->repeat_count = (void *)0;
  ast->repeat_body = (void *)0;
  ast->should_break = 0;
  ast->should_skip = 0;
  ast->while_body = (void *)0;
  ast->while_cond = (void *)0;
  ast->unop_op = 0;
  ast->unop_body = (void *)0;
  ast->param_name = NULL;
  ast->should_return = 0;
  ast->return_value = NULL;
  return ast;
}