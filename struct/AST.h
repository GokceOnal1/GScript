#ifndef GSCRIPT2_AST_H
#define GSCRIPT2_AST_H
#include <stdlib.h>
typedef struct AST_s {
    enum { AST_VAR_DEF, AST_FUNC_DEF, AST_VAR, AST_FUNC_CALL, AST_STR, AST_COMPOUND, AST_NOOP, AST_NUM, AST_BINOP, AST_BOOL, AST_IF, AST_REPEAT, AST_OUT, AST_SKIP, AST_REASSIGN, AST_WHILE, AST_UNOP, AST_PARAM, AST_RETURN, AST_LIST, AST_LISTINDX, AST_OBJREF, AST_GROUP, AST_OBJ_DEF, AST_DOT, AST_OBJACCESS, AST_OBJ, AST_IMPORT, AST_OBJ_REASSIGN, AST_LIST_ARROW, AST_LIST_REASSIGN } type;
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
    char *func_def_file;

    //AST_VAR
    char* var_name;
    unsigned var_references;

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

    //AST_IF
    struct AST_s** if_bodies;
    size_t if_bodies_size;
    struct AST_s** if_conds;
    size_t if_conds_size;
    struct AST_s* else_body;

    //AST_REPEAT
    struct AST_s* repeat_count;
    struct AST_s* repeat_body;

    //AST_OUT
    unsigned should_break;

    //AST_SKIP
    unsigned should_skip;

    //AST_WHILE
    struct AST_s* while_body;
    struct AST_s* while_cond;

    //AST_UNOP
    int unop_op;
    struct AST_s* unop_body;

    //AST_PARAM
    char* param_name;

    //AST_RETURN
    unsigned should_return;
    struct AST_s* return_value;

    //AST_LIST
    unsigned list_size;
    struct AST_s** list_contents;
    unsigned list_index;

    //AST_LISTINDX
    struct AST_s* list_reference;
    unsigned list_sub_index;

    //AST_OBJREF
    char* objref_name;

    //AST_GROUP
    char* group_name;

    //AST_OBJ_DEF
    char *obj_name;
    struct AST_s ** fields;
    size_t fields_size;
    struct AST_s ** methods;
    size_t methods_size;
    struct AST_s ** objects;
    size_t objects_size;


    //AST_OBJACCESS
    struct AST_s *objaccess_right;
    struct AST_s *objaccess_left;

    //AST_OBJ
    struct AST_s *obj_objdef;

    //AST_IMPORT
    char *import_val;
    unsigned import_is_builtin;

    //AST_OBJ_REASSIGN
    struct AST_s *obj_reassign_left;
    struct AST_s *obj_reassign_right;

    //AST_LIST_ARROW
    struct AST_s *list_arrow_left;
    struct AST_s *list_arrow_right;

    //AST_LIST_REASSIGN
    struct AST_s *list_reassign_left;
    struct AST_s *list_reassign_right;
} AST;

AST* ast_init(int type);
#endif //GSCRIPT2_AST_H
