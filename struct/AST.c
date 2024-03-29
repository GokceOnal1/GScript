#include "AST.h"
#include <errno.h>
#include <stdio.h>
AST *ast_init(int type) {
    AST *ast = calloc(1, sizeof(AST));
    if(ast == NULL) {
        if(errno == ENOMEM) {
            printf(" No memory!! ");
        }
        return NULL;
    }
    ast->type = type;
    ast->scope = NULL;
    ast->var_def_var_name = NULL;
    ast->var_def_value = NULL;
    ast->func_def_body = NULL;
    ast->func_def_name = NULL;
    ast->func_def_args = NULL;
    ast->func_def_args_size = 0;
    ast->var_name = NULL;
    ast->func_call_name = NULL;
    ast->func_call_args = NULL;
    ast->func_call_args_size = 0;
    ast->str_value = NULL;
    ast->compound_value = NULL;
    ast->compound_size = 0;
    ast->line = 0;
    ast->num_value = 0;
    ast->left = NULL;
    ast->op = 0;
    ast->right = NULL;
    ast->b_value = 0;
    ast->if_bodies = NULL;
    ast->if_bodies_size = 0;
    ast->if_conds_size = 0;
    ast->if_conds = NULL;
    ast->else_body = NULL;
    ast->repeat_count = NULL;
    ast->repeat_body = NULL;
    ast->should_break = 0;
    ast->should_skip = 0;
    ast->while_body = NULL;
    ast->while_cond = NULL;
    ast->unop_op = 0;
    ast->unop_body = NULL;
    ast->param_name = NULL;
    ast->should_return = 0;
    ast->return_value = NULL;
    ast->list_contents = NULL;
    ast->list_size = 0;
    ast->list_index = 0;
    ast->list_reference = NULL;
    ast->list_sub_index = 0;
    ast->objref_name = NULL;
    ast->group_name = NULL;
    ast->obj_name = NULL;
    ast->fields = NULL;
    ast->fields_size = 0;
    ast->methods = NULL;
    ast->methods_size = 0;
    ast->objects = NULL;
    ast->objects_size = 0;
    ast->objaccess_left = NULL;
    ast->objaccess_right = NULL;
    ast->obj_objdef = NULL;
    ast->import_val = NULL;
    ast->import_is_builtin = 0;
    ast->obj_reassign_left = NULL;
    ast->obj_reassign_right = NULL;
    ast->func_def_file = NULL;
    ast->var_references = 0;
    return ast;
}