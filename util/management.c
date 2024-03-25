//
// Created by obona on 3/18/2024.
//

#include "management.h"
#include "../struct/AST.h"

#define qcpy(x) to->x = from->x
#define qcpym(x) if(from->x != NULL) { \
to->x = ast_init(0);           \
MEM_copy_ast(to->x, from->x);          \
} else to->x = NULL
#define nf(x) node->x = NULL
#define af(x) node->x = 0
#define qf(x) if(1) { \
MEM_free_ast(node->x); }
#define qqf(x, s) if(node->x != NULL) { \
MEM_free_ast_arr(node->x, node->s); }
#define qp(s, x) printf("\n"); \
for(int i = 0; i < tab_count; i++) printf("\t"); \
printf( s, curr->x)

char* ast_str_mem[31] = { "variable definition", "function definition", "variable", "function call", "string", "compound statements", "no operation", "number", "binary operation", "boolean", "if statement", "repeat statement", "out statement", "skip statement", "variable reassignment", "while statement", "unary operation", "parameter", "return statement", "list", "list index", "object reference", "group", "object definition", "dot", "object access", "object", "import statement", "object reassignment", "list arrow", "list reassignment"};
/*Deep free of NODE*/
/*UPDATE AS AST'S ARE ADDED*/
void MEM_free_ast(AST* node) {
    if(node == NULL) return;
    if(node->type == AST_NUM) {
        //printf(" num encounter ");
    }
    switch(node->type) {
        case AST_VAR_DEF:
            //nf(var_def_var_name);
            qf(var_def_value)
            break;
        case AST_FUNC_DEF:
            qf(func_def_body)
            nf(func_def_name);
            qqf(func_def_args, func_def_args_size)
            af(func_def_args_size);
            nf(func_def_file);
            break;
        case AST_VAR:
            nf(var_name);
            af(var_references);
            break;
        case AST_FUNC_CALL:
            nf(func_call_name);
            qqf(func_call_args, func_call_args_size)
            af(func_call_args_size);
            break;
        case AST_STR:
            nf(str_value);
            break;
        case AST_COMPOUND:
            qqf(compound_value, compound_size)
            af(compound_size);
            break;
        case AST_NUM:

            af(num_value);
            break;
        case AST_BINOP:
            qf(left)
            af(op);
            qf(right)
            break;
        case AST_BOOL:
            af(b_value);
            break;
        case AST_IF:
            qqf(if_bodies, if_bodies_size)
            af(if_bodies_size);
            qqf(if_conds, if_conds_size)
            af(if_conds_size);
            qf(else_body)
            break;
        case AST_REPEAT:
            qf(repeat_count)
            qf(repeat_body)
            break;
        case AST_OUT:
            af(should_break);
            break;
        case AST_SKIP:
            af(should_skip);
            break;
        case AST_WHILE:
            qf(while_body)
            qf(while_cond)
            break;
        case AST_UNOP:
            af(unop_op);
            qf(unop_body)
            break;
        case AST_PARAM:
            nf(param_name);
            break;
        case AST_RETURN:
            af(should_return);
            qf(return_value)
            break;
        case AST_LIST:
            af(list_size);
            qqf(list_contents, list_size)
            af(list_index);
            break;
        case AST_LISTINDX:
            qf(list_reference)
            af(list_sub_index);
            break;
        case AST_OBJREF:
            nf(objref_name);
            break;
        case AST_GROUP:
            nf(group_name);
            break;
        case AST_OBJ_DEF:
            nf(obj_name);
            qqf(fields, fields_size)
            af(fields_size);
            qqf(methods, methods_size)
            af(methods_size);
            qqf(objects, objects_size)
            af(objects_size);
            break;
        case AST_OBJACCESS:
            qf(objaccess_right)
            qf(objaccess_left)
            break;
        case AST_OBJ:
            qf(obj_objdef)
            break;
        case AST_IMPORT:
            nf(import_val);
            af(import_is_builtin);
            break;
        case AST_OBJ_REASSIGN:
            qf(obj_reassign_left)
            qf(obj_reassign_right)
            break;
        case AST_LIST_ARROW:
            qf(list_arrow_left)
            qf(list_arrow_right)
            break;
        case AST_LIST_REASSIGN:
            qf(list_reassign_left)
            qf(list_reassign_right)
            break;
        default:
            node = NULL;
            break;
    }
    if(node!=NULL) free(node);
    //printf(" after ");
}

/*Deep copy of FROM to TO*/
/*Note: Unsure about copying char* vs malloc'd char**/
/*UPDATE AS AST'S ARE ADDED*/
void MEM_copy_ast(AST* to, AST* from) {
    if(to == NULL) return;
    //copy scope as well
    //printf(" %d ", from->type);
    switch(from->type) {
        case AST_VAR_DEF:
            qcpy(var_def_var_name);
            qcpym(var_def_value);
            break;
        case AST_FUNC_DEF:
            qcpym(func_def_body);
            qcpy(func_def_name);
            to->func_def_args = calloc(from->func_def_args_size, sizeof(AST*));
            MEM_copy_ast_arr(to->func_def_args, from->func_def_args, from->func_def_args_size);
            qcpy(func_def_args_size);
            qcpy(func_def_file);
            break;
        case AST_VAR:
            qcpy(var_name);
            qcpy(var_references);
            break;
        case AST_FUNC_CALL:
            qcpy(func_call_name);
            to->func_call_args = calloc(from->func_call_args_size, sizeof(AST*));
            MEM_copy_ast_arr(to->func_call_args, from->func_call_args, from->func_call_args_size);
            qcpy(func_call_args_size);
            break;
        case AST_STR:
            qcpy(str_value);
            break;
        case AST_COMPOUND:
            to->compound_value = calloc(from->compound_size, sizeof(AST*));
            MEM_copy_ast_arr(to->compound_value, from->compound_value, from->compound_size);
            qcpy(compound_size);
            break;
        case AST_NUM:
            qcpy(num_value);
            break;
        case AST_BINOP:
            qcpym(left);
            qcpy(op);
            qcpym(right);
            break;
        case AST_BOOL:
            qcpy(b_value);
            break;
        case AST_IF:
            to->if_bodies = calloc(from->if_bodies_size, sizeof(AST*));
            MEM_copy_ast_arr(to->if_bodies, from->if_bodies, from->if_bodies_size);
            qcpy(if_bodies_size);
            to->if_conds = calloc(from->if_conds_size, sizeof(AST*));
            MEM_copy_ast_arr(to->if_conds, from->if_conds, from->if_conds_size);
            qcpy(if_conds_size);
            qcpym(else_body);
            break;
        case AST_REPEAT:
            qcpym(repeat_count);
            qcpym(repeat_body);
            break;
        case AST_OUT:
            qcpy(should_break);
            break;
        case AST_SKIP:
            qcpy(should_skip);
            break;
        case AST_WHILE:
            qcpym(while_body);
            qcpym(while_cond);
            break;
        case AST_UNOP:
            qcpy(unop_op);
            qcpym(unop_body);
            break;
        case AST_PARAM:
            qcpy(param_name);
            break;
        case AST_RETURN:
            qcpy(should_return);
            qcpym(return_value);
            break;
        case AST_LIST:
            qcpy(list_size);
            to->list_contents = calloc(from->list_size, sizeof(AST*));
            MEM_copy_ast_arr(to->list_contents, from->list_contents, from->list_size);
            qcpy(list_index);
            break;
        case AST_LISTINDX:
            qcpym(list_reference);
            qcpy(list_sub_index);
            break;
        case AST_OBJREF:
            qcpy(objref_name);
            break;
        case AST_GROUP:
            qcpy(group_name);
            break;
        case AST_OBJ_DEF:
            qcpy(obj_name);
            to->fields = calloc(from->fields_size, sizeof(AST*));
            MEM_copy_ast_arr(to->fields, from->fields, from->fields_size);
            qcpy(fields_size);
            to->methods = calloc(from->methods_size, sizeof(AST*));
            MEM_copy_ast_arr(to->methods, from->methods, from->methods_size);
            qcpy(methods_size);
            to->objects = calloc(from->objects_size, sizeof(AST*));
            MEM_copy_ast_arr(to->objects, from->objects, from->objects_size);
            qcpy(objects_size);
            break;
        case AST_OBJACCESS:
            qcpym(objaccess_right);
            qcpym(objaccess_left);
            break;
        case AST_OBJ:
            qcpym(obj_objdef);
            break;
        case AST_IMPORT:
            qcpy(import_val);
            qcpy(import_is_builtin);
            break;
        case AST_OBJ_REASSIGN:
            qcpym(obj_reassign_left);
            qcpym(obj_reassign_right);
            break;
        case AST_LIST_ARROW:
            qcpym(list_arrow_left);
            qcpym(list_arrow_right);
            break;
        case AST_LIST_REASSIGN:
            qcpym(list_reassign_left);
            qcpym(list_reassign_right);
            break;
        default:
            to = NULL;
            break;
    }
    if(to!=NULL) qcpy(type);
}

void MEM_copy_ast_arr(AST** to, AST** from, size_t len) {
    for(size_t i = 0; i < len; i++) {
        to[i] = ast_init(0);
        if(from[i] != NULL) {
            MEM_copy_ast(to[i], from[i]);
        } else to[i] = NULL;
    }
}

void MEM_free_ast_arr(AST** node, size_t len) {
    for(size_t i = 0; i < len; i++) {
        if(node[i] != NULL) {
            MEM_free_ast(node[i]);
        }
    }
}
void visualize_ast(AST* curr, unsigned tab_count) {
    if(curr == NULL) { printf("\n--NULL--"); return; }
    printf("\n");
    for(int i = 0; i < tab_count; i++) printf("\t");
    printf("AST type %d (%s)", curr->type, ast_str_mem[curr->type]);
    switch(curr->type) {
        case AST_VAR_DEF:
            qp("var name: %s",var_def_var_name);
            visualize_ast(curr->var_def_value, tab_count+1);
            break;
        case AST_FUNC_DEF:
            visualize_ast(curr->func_def_body, tab_count+1);
            qp("func def name: %s",func_def_name);
            visualize_ast_arr(curr->func_def_args, curr->func_def_args_size, tab_count+1);
            qp("func def args size: %zu",func_def_args_size);
            qp("func def file: %s",func_def_file);
            break;
        case AST_VAR:
            qp("var name: %s", var_name);
            qp("var refs: %d",var_references);
            break;
        case AST_FUNC_CALL:
            qp("func call name: %s", func_call_name);
            visualize_ast_arr(curr->func_call_args, curr->func_call_args_size, tab_count+1);
            qp("func call args size: %zu", func_call_args_size);
            break;
        case AST_STR:
            qp("str val: %s" ,str_value);
            break;
        case AST_COMPOUND:
            visualize_ast_arr(curr->compound_value, curr->compound_size, tab_count+1);
            qp("comp size: %zu", compound_size);
            break;
        case AST_NUM:
            qp("num val: %f" ,num_value);
            break;
        case AST_BINOP:
            visualize_ast(curr->left, tab_count+1);
            qp("op: %d", op);
            visualize_ast(curr->right, tab_count+1);
            break;
        case AST_BOOL:
            qp("b value: %d", b_value);
            break;
        case AST_IF:
            visualize_ast_arr(curr->if_bodies, curr->if_bodies_size, tab_count+1);
            qp("if bodies size: %zu", if_bodies_size);
            visualize_ast_arr(curr->if_conds, curr->if_conds_size, tab_count+1);
            qp("if conds size: %zu", if_conds_size);
            visualize_ast(curr->else_body, tab_count+1);
            break;
        case AST_REPEAT:
            visualize_ast(curr->repeat_count, tab_count+1);
            visualize_ast(curr->repeat_body, tab_count+1);
            break;
        case AST_OUT:
            qp("should break: %d", should_break);
            break;
        case AST_SKIP:
            qp("should skip: %d", should_skip);
            break;
        case AST_WHILE:
            visualize_ast(curr->while_body, tab_count+1);
            visualize_ast(curr->while_cond, tab_count+1);
            break;
        case AST_UNOP:
            qp("unop op: %d", unop_op);
            visualize_ast(curr->unop_body, tab_count+1);
            break;
        case AST_PARAM:
            qp("param name: %s", param_name);
            break;
        case AST_RETURN:
            qp("should return: %d", should_return);
            visualize_ast(curr->return_value, tab_count+1);
            break;
        case AST_LIST:
            qp("list size: %d",list_size);
            visualize_ast_arr(curr->list_contents, curr->list_size, tab_count+1);
            qp("list index: %d", list_index);
            break;
        case AST_LISTINDX:
            visualize_ast(curr->list_reference, tab_count+1);
            qp("list sub index: %d", list_sub_index);
            break;
        case AST_OBJREF:
            qp("objref name: %s", objref_name);
            break;
        case AST_GROUP:
            qp("group name: %s", group_name);
            break;
        case AST_OBJ_DEF:
            qp("obj name: %s", obj_name);
            visualize_ast_arr(curr->fields, curr->fields_size, tab_count+1);
            qp("fields size: %zu", fields_size);
            visualize_ast_arr(curr->methods, curr->methods_size, tab_count+1);
            qp("methods size: %zu", methods_size);
            visualize_ast_arr(curr->objects, curr->objects_size, tab_count+1);
            qp("objects size: %zu", objects_size);
            break;
        case AST_OBJACCESS:
            visualize_ast(curr->objaccess_right, tab_count+1);
            visualize_ast(curr->objaccess_left, tab_count+1);
            break;
        case AST_OBJ:
            visualize_ast(curr->obj_objdef, tab_count+1);
            break;
        case AST_IMPORT:
            qp("import val: %s", import_val);
            qp("import is builtin: %d", import_is_builtin);
            break;
        case AST_OBJ_REASSIGN:
            visualize_ast(curr->obj_reassign_left, tab_count+1);
            visualize_ast(curr->obj_reassign_right, tab_count+1);
            break;
        case AST_LIST_ARROW:
            visualize_ast(curr->list_arrow_left, tab_count+1);
            visualize_ast(curr->list_arrow_right, tab_count+1);
            break;
        case AST_LIST_REASSIGN:
            visualize_ast(curr->list_reassign_left, tab_count+1);
            visualize_ast(curr->list_reassign_right, tab_count+1);
            break;
        default:
            printf("--NULL--");
            break;
    }
    printf("\n");

}
void visualize_ast_arr(AST** arr, size_t len, unsigned tab_count) {
    printf("\n");
    for(int i = 0; i < tab_count; i ++) printf("\t");
    printf("WITHIN ARR--");
    for(size_t i = 0; i < len; i++) {
        visualize_ast(arr[i], tab_count);
    }
    printf("\n");
    for(int i = 0; i < tab_count; i ++) printf("\t");
    printf("--END ARR");
}