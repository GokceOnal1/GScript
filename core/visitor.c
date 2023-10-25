//
// Created by obona on 9/7/2023.
//

#include "visitor.h"
#include "../extra/stdfunc.h"

#define _MAXLINE 1000
#define __err(fmt, ...) _GSERR_s(visitor->e, line, fmt, __VA_ARGS__)

/*Constructs a new visitor*/
Visitor *visitor_init(ErrorStack *errorstack) {
    Visitor *visitor = calloc(1, sizeof(Visitor));
    visitor->var_defs = (void *)0;
    visitor->var_defs_size = 0;
    visitor->current_scope = scope_init(NULL, errorstack);
    visitor->e = errorstack;
    srand((unsigned)time(NULL));
    return visitor;
}
/*Tells the visitor to visit a syntax tree node*/
AST *visitor_visit(Visitor *visitor, AST *node) {
    switch (node->type) {
        case AST_VAR_DEF:
            return visitor_visit_var_def(visitor, node);
            break;
        case AST_REASSIGN:
            return visitor_visit_var_reassign(visitor, node);
            break;
        case AST_FUNC_DEF:
            return visitor_visit_func_def(visitor, node);
            break;
        case AST_VAR:
            return visitor_visit_var(visitor, node);
            break;
        case AST_OBJREF:

        case AST_FUNC_CALL:
            return visitor_visit_func_call(visitor, node);
            break;
        case AST_STR:
            return visitor_visit_str(visitor, node);
            break;
        case AST_NUM:
            return visitor_visit_num(visitor, node);
            break;
        case AST_LIST: return node;
        case AST_BINOP:
            return visitor_visit_binop(visitor, node);
            break;
        case AST_BOOL:
            return visitor_visit_bool(visitor, node);
            break;
        case AST_IF:
            return visitor_visit_if_statement(visitor, node);
            break;
        case AST_REPEAT:
            return visitor_visit_repeat_statement(visitor, node);
            break;
        case AST_WHILE:
            return visitor_visit_while_statement(visitor, node);
            break;
        case AST_OUT:
            return node;
            break;
        case AST_RETURN:
            return node;
        case AST_UNOP:
            return visitor_visit_unop(visitor, node);
        case AST_LISTINDX:
            return visitor_visit_listindx(visitor, node);
        case AST_OBJ: {
            return visitor_visit_obj_def(visitor, node);
        }
        case AST_COMPOUND:
            return visitor_visit_compound(visitor, node);
            break;
        case AST_NOOP:
            return node;
            break;
    }
    char d[100];
    sprintf(d, "GS201 - Uncaught statement of type %d", node->type);
    _GSERR(node->line, d);
    exit(1);
    return ast_init(AST_NOOP);
}
/*Tells the visitor to interpret a variable definition and store it to memory*/
AST *visitor_visit_var_def(Visitor *visitor, AST *node) {
    AST* val = visitor_visit(visitor, node->var_def_value);
    AST* def = ast_init(AST_VAR_DEF);
    def->var_def_var_name = node->var_def_var_name;
    def->var_def_value = val;
    scope_add_var_def(visitor->current_scope, def);
    return node;
}
AST *visitor_visit_var_reassign(Visitor *visitor, AST *node) {
    // printf("\n ENCOUNTERED REASSIGN with value type %d\n", node->var_def_value->type);
    AST* val = visitor_visit(visitor, node->var_def_value);
    // printf("\nVisitng reassing node, value is %f", val->num_value);
    AST* newval = ast_init(AST_VAR_DEF);
    newval->var_def_var_name = node->var_def_var_name;
    newval->var_def_value = val;
    scope_set_var(visitor->current_scope, newval);
    // printf(" %f ", scope_get_var_def(visitor->current_scope, "a")->num_value);
    return node;
}
AST* visitor_visit_func_def(Visitor *visitor, AST* node) {
    node->scope->parent = visitor->current_scope;
    visitor->current_scope = scope_init(visitor->current_scope, visitor->e);
    for(int i = 0; i < node->func_def_args_size; i++) {
        scope_add_var_def(visitor->current_scope, node->func_def_args[i]);
    }
    node->scope = visitor->current_scope;
    visitor->current_scope = visitor->current_scope->parent;
    scope_add_function_definition(visitor->current_scope, node);
    return node;
}
/*Tells the visitor to interpret a variable value and store it to memory*/
AST *visitor_visit_var(Visitor *visitor, AST *node) {
    if(strcmp(node->var_name, "pi") == 0) {
        AST* result = ast_init(AST_NUM);
        result->num_value = 3.1415926535898;
        return result;
    }
    AST* val = scope_get_var_def(visitor->current_scope, node->var_name);
    if(val == NULL) {
        char d[100];
        sprintf(d, "GS303 - Variable '%s' does not exist in the current scope", node->var_name);
        _GSERR_s(visitor->e, node->line, d);
    } else {
        return val;
    }
}
/*Tells the visitor to interpret a function call and execute it*/
AST *visitor_visit_func_call(Visitor *visitor, AST *node) {
//  if(node->should_break) {printf("RETURN") ; exit(1);}
    if (strcmp(node->func_call_name, "write") == 0) {
        return std_func_write(visitor, node->func_call_args,
                              node->func_call_args_size, node->line);
    }
    if (strcmp(node->func_call_name, "read") == 0) {
        return std_func_read(visitor, node->func_call_args,
                             node->func_call_args_size, node->line);
    }
    if (strcmp(node->func_call_name, "toupper") == 0) {
        return std_func_toupper(visitor, node->func_call_args, node->func_call_args_size, node->line);
    }
    if (strcmp(node->func_call_name, "tolower") == 0) {
        return std_func_tolower(visitor, node->func_call_args, node->func_call_args_size, node->line);
    }
    if (strcmp(node->func_call_name, "randnum") == 0) {
        return std_func_randnum(visitor, node->func_call_args, node->func_call_args_size, node->line);
    }
    if (strcmp(node->func_call_name, "str") == 0) {
        return visitor_convert_to_str(visitor, node->func_call_args[0]);
    }
    if (strcmp(node->func_call_name, "num") == 0) {
        return std_func_num(visitor, node->func_call_args, node->func_call_args_size, node->line);
    }
    if (strcmp(node->func_call_name, "len") == 0) {
        return std_func_len(visitor, node->func_call_args, node->func_call_args_size, node->line);
    }
    if (strcmp(node->func_call_name, "strindx") == 0) {
        return std_func_strindx(visitor, node->func_call_args, node->func_call_args_size, node->line);
    }
    if (strcmp(node->func_call_name, "listindx") == 0) {
        return std_func_listindx(visitor, node->func_call_args, node->func_call_args_size, node->line);
    }
    AST* fdef = scope_get_function_definition(visitor->current_scope, node->func_call_name);
    if(fdef != NULL) {
        Scope* prev = visitor->current_scope;
        visitor->current_scope = fdef->scope;
        //printf("%d", fdef->func_def_args_size);
        AST* res = NULL;
        AST** prev_args = calloc(fdef->func_def_args_size, sizeof(AST*));
        for(int i = 0; i < fdef->func_def_args_size; i++) {
            prev_args[i] = ast_init(AST_VAR_DEF);
            prev_args[i]->var_def_value = scope_get_var_def(visitor->current_scope, fdef->func_def_args[i]->var_def_var_name);
            prev_args[i]->var_def_var_name = fdef->func_def_args[i]->var_def_var_name;

            AST* arg = ast_init(AST_VAR_DEF);
            arg->var_def_var_name = fdef->func_def_args[i]->var_def_var_name;
            visitor->current_scope = prev;
            arg->var_def_value = visitor_visit(visitor,node->func_call_args[i]);
            visitor->current_scope = fdef->scope;
            // printf("%f", arg->var_def_value->num_value);
            // printf("\n%s", arg->var_def_var_name);
            scope_set_var(fdef->scope, arg);
            // printf("%f", arg->var_def_value->num_value);
        }
        res = visitor_visit(visitor, fdef->func_def_body);
        AST* returned = NULL;
        if(res->return_value) {
            returned = visitor_visit(visitor, res->return_value);
        } else {
            returned = ast_init(AST_NOOP);
        }

        for(int i = 0; i < fdef->func_def_args_size; i++) {
            // printf("%s\n",prev_args[i]->var_def_var_name);
            scope_set_var(fdef->scope, prev_args[i]);
        }
        visitor->current_scope = prev;
//    if(res->should_break) exit(1);
//    if(res->should_return) exit(1);
        return returned;
    }
    char d[100];
    sprintf(d, "GS503 - Undefined function '%s'", node->func_call_name);
    _GSERR(node->line, d);
    exit(1);
}
/*Tells the visitor to interpret a string AST node*/
AST *visitor_visit_str(Visitor *visitor, AST *node) { return node; }
/*Tells the visitor to begin the recursive process of interpreting the root
 * syntax tree*/
AST *visitor_visit_num(Visitor *visitor, AST *node) {
    return node;
}
AST *visitor_visit_binop(Visitor *visitor, AST *node) {
    AST* left = visitor_visit(visitor, node->left);
    AST* right = visitor_visit(visitor, node->right);
    switch(node->op) {
        case T_PLUS: {
            AST *result = NULL;
            if(left->type == AST_NUM && right->type == AST_NUM) {
                result = ast_init(AST_NUM);
                result->should_break = left->should_break;
                result->num_value = left->num_value + right->num_value;
            } else if(left->type == AST_STR && right->type == AST_STR) {
                result = ast_init(AST_STR);
                result->str_value = left->str_value;
                strcat(result->str_value, right->str_value);
            } else {
                result = ast_init(AST_STR);
                result->str_value = malloc(10 * sizeof(char));
                strcpy(result->str_value, "undefined\0");
            }
            result->line = node->line;
            return result;
        }
        case T_MIN: {
            AST *result = NULL;
            if(left->type == AST_NUM && right->type == AST_NUM) {
                //printf("\nSubbing from %f", left->num_value);
                result = ast_init(AST_NUM);
                result->num_value = left->num_value - right->num_value;
            } else {
                result = ast_init(AST_STR);
                result->str_value = malloc(10 * sizeof(char));
                strcpy(result->str_value, "undefined\0");
            }
            result->line = node->line;
            return result;
        }
        case T_MUL: {
            AST *result = ast_init(AST_NUM);
            result->num_value = left->num_value * right->num_value;
            return result;
        }
        case T_DIV: {
            if(right->num_value == 0) {
                _GSERR(node->line, "GS601 - Dividing by zero is illegal");
            }
            AST *result = ast_init(AST_NUM);
            result->num_value = left->num_value / right->num_value;
            return result;
        }
        case T_MOD: {
            if(right->num_value == 0) {
                _GSERR(right->line, "GS601 - Dividing by zero is illegal");
            }
            AST *result = ast_init(AST_NUM);
            result->num_value = (int)left->num_value % (int)right->num_value;
            return result;
        }
        case T_EE: {
            AST *result = ast_init(AST_BOOL);
            if(left->type == AST_NUM && right->type == AST_NUM) {
                result->b_value = left->num_value == right->num_value;
            } else if(left->type == AST_BOOL && right->type == AST_BOOL) {
                result->b_value = left->b_value == right->b_value;
            } else if(left->type == AST_STR && right->type == AST_STR) {
                result->b_value = (strcmp(left->str_value, right->str_value) == 0);
            }
            return result;
        }
        case T_NEQ: {
            AST *result = ast_init(AST_BOOL);
            if(left->type == AST_NUM && right->type == AST_NUM) {
                result->b_value = left->num_value != right->num_value;
            } else if(left->type == AST_BOOL && right->type == AST_BOOL) {
                result->b_value = left->b_value != right->b_value;
            } else if(left->type == AST_STR && right->type == AST_STR) {
                result->b_value = (strcmp(left->str_value, right->str_value) != 0);
            }
            return result;
        }
        case T_LT: {
            AST *result = ast_init(AST_BOOL);
            result->b_value = left->num_value < right->num_value;
            return result;
        }
        case T_LTE: {
            AST *result = ast_init(AST_BOOL);
            result->b_value = left->num_value <= right->num_value;
            return result;
        }
        case T_GT: {
            AST *result = ast_init(AST_BOOL);
            result->b_value = left->num_value > right->num_value;
            return result;
        }
        case T_GTE: {
            AST *result = ast_init(AST_BOOL);
            result->b_value = left->num_value >= right->num_value;
            return result;
        }
        case T_AND: {
            AST *result = ast_init(AST_BOOL);
            result->b_value = left->b_value && right->b_value;
            return result;
        }
        case T_OR: {
            AST *result = ast_init(AST_BOOL);
            result->b_value = left->b_value || right->b_value;
            return result;
        }
        default: {
            _GSERR(0, "Error while interpreting <INTERNAL>");
            return ast_init(AST_NOOP);
        }
    }
}
AST *visitor_visit_bool(Visitor *visitor, AST *node) {
    return node;
}
AST *visitor_visit_if_statement(Visitor *visitor, AST *node) {
    for(int i = 0; i < node->if_conds_size; i++) {
        AST* cond = visitor_visit(visitor, node->if_conds[i]);
        if(cond->type != AST_BOOL) printf("Expected conditional expression");
        if(cond->b_value) {
            AST* res = visitor_visit(visitor, node->if_bodies[i]);
            return res;
        }
    }
    if(node->else_body != NULL) {
        AST* res = visitor_visit(visitor, node->else_body);
        return res;
    }
    return node;
}
AST *visitor_visit_repeat_statement(Visitor *visitor, AST *node) {
    AST *count = visitor_visit(visitor, node->repeat_count);
    for(int i = 0; i < count->num_value; i++) {
        AST* stat = visitor_visit(visitor, node->repeat_body);
        //  if(scope_get_var_def(visitor->current_scope, "x")->num_value == 9) printf("ok");
        //  printf(" %f ", scope_get_var_def(visitor->current_scope, "x")->var_def_value->num_value);
        if(stat->should_break) break;
    }
    return ast_init(AST_NOOP);
}
AST *visitor_visit_while_statement(Visitor *visitor, AST *node) {
    while(visitor_visit(visitor, node->while_cond)->b_value) {
        visitor_visit(visitor, node->while_body);
    }
    return ast_init(AST_NOOP);
}
AST *visitor_visit_unop(Visitor *visitor, AST *node) {
    AST* val = visitor_visit(visitor, node->unop_body);
    AST* res = NULL;
    switch(node->unop_op) {
        case T_MIN:
            res = ast_init(AST_NUM);
            res->num_value = -(val->num_value);
            break;
        case T_NOT:
            res = ast_init(AST_BOOL);
            res->b_value = !(val->b_value);
            break;
    }
    return res;
}
AST* visitor_visit_listindx(Visitor *visitor, AST *node) {
    /* AST* res = ast_init(AST_LISTINDX);
     res->list_reference = visitor_visit(visitor, node->list_reference);
     res->list_sub_index = node->list_sub_index;*/
    AST* list = visitor_visit(visitor, node->list_reference);
    AST* res = visitor_visit(visitor, list->list_contents[node->list_sub_index]);
    return res;
}
AST *visitor_convert_to_str(Visitor *visitor, AST *node) {
    AST* str_node = ast_init(AST_STR);
    char* buffer = calloc(1000, sizeof(char));
    buffer[999] = '\0';
    AST* v_node = visitor_visit(visitor, node);
    switch(v_node->type) {
        case AST_STR:
            //printf(" v node: %s ", v_node->str_value);
            strcat(buffer, v_node->str_value);
           // printf(" buffer: %s", buffer);
            break;
        case AST_NUM: {
            char n[25];
           // printf(">>%f<<", v_node->num_value);
            //gcvt(v_node->num_value,15,n);
            sprintf(n, "%f", v_node->num_value);
            //printf(" %s ", n);
            strcat(buffer, n);
            break;
        }
        case AST_BOOL: {
            strcat(buffer, v_node->b_value ? "true" : "false");
            break;
        }
        case AST_LIST: {
           // printf("\n>>>%s", buffer);
            strcat(buffer, "[");
            for(int i = 0; i < v_node->list_size; i++) {
                if(i != 0)  strcat(buffer, ", ");
                strcat(buffer, visitor_convert_to_str(visitor, v_node->list_contents[i])->str_value);

            }
            strcat(buffer, "]");
            break;
        }
        case AST_LISTINDX: {
            //  AST* val = v_node->list_reference->list_contents[v_node->list_sub_index];
            strcat(buffer, visitor_convert_to_str(visitor, v_node)->str_value);
            break;
        }
        case AST_OBJ: {
            strcat(buffer, "{ ");
            for(int i = 0; i < v_node->fields_size; i++) {
                if(i != 0)  strcat(buffer, ", ");
                strcat(buffer, "Field ");
                strcat(buffer, v_node->fields[i]->var_def_var_name);
                strcat(buffer, ": ");
                strcat(buffer, visitor_convert_to_str(visitor, v_node->fields[i]->var_def_value)->str_value);
            }
            for(int i = 0; i < v_node->methods_size; i++) {
                strcat(buffer, ", ");
                strcat(buffer, "Method ");
                strcat(buffer, v_node->methods[i]->func_def_name);
            }
            strcat(buffer, " }");
            break;
        }
        default: {
            strcat(buffer, "NULL");
            break;
        }
    }
    str_node->str_value = buffer;
   // printf("%s",str_node->str_value);
    return str_node;
}
AST *visitor_visit_obj_def(Visitor *visitor, AST *node) {
    //scope_add_obj_def(visitor->current_scope, node);
    return node;
}
AST *visitor_visit_compound(Visitor *visitor, AST *node) {
    Scope* compound_scope = scope_init(visitor->current_scope, visitor->e);
    visitor->current_scope = compound_scope;
    for (int i = 0; i < node->compound_size; i++) {
        AST* stat = visitor_visit(visitor, node->compound_value[i]);
        if(stat->should_return || stat->should_break) return stat;
    }
    visitor->current_scope = visitor->current_scope->parent;
    return node;
}