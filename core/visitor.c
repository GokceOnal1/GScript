//
// Created by obona on 9/7/2023.
//

#include "visitor.h"
#include "../extra/stdfunc.h"
#include "parser.h"

#define _MAXLINE 1000
#define __err(fmt, ...) _GSERR_s(visitor->e, line, fmt, __VA_ARGS__)
char* ast_str[31] = { "variable definition", "function definition", "variable", "function call", "string", "compound statements", "no operation", "number", "binary operation", "boolean", "if statement", "repeat statement", "out statement", "skip statement", "variable reassignment", "while statement", "unary operation", "parameter", "return statement", "list", "list index", "object reference", "group", "object definition", "dot", "object access", "object", "import statement", "object reassignment", "list arrow", "list reassignment"};
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
           // printf("func def");
            return visitor_visit_func_def(visitor, node);
            break;
        case AST_VAR:
            return visitor_visit_var(visitor, node);
            break;
        case AST_OBJREF:

        case AST_FUNC_CALL:
           // printf("func call");
            return visitor_visit_func_call(visitor, node, NULL);
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
           // printf("at return file is %s ", visitor->e->curr_file);
            return node;
        case AST_UNOP:
            return visitor_visit_unop(visitor, node);
        case AST_LISTINDX:
            return visitor_visit_listindx(visitor, node);
        case AST_OBJ_DEF: {
            //printf("obj def");
            return visitor_visit_obj_def(visitor, node);
        }
        case AST_OBJACCESS: {
           // printf("obj acc");
            return visitor_visit_objaccess(visitor, node);
        }
        case AST_OBJ: return node;
        case AST_OBJ_REASSIGN: {
            return visitor_visit_obj_reassign(visitor, node);
        }
        case AST_LIST_REASSIGN : {
            //printf("visiting");
            return visitor_visit_list_reassign(visitor, node);
        }
        case AST_IMPORT: {
            return visitor_visit_import(visitor, node);
        }
        case AST_LIST_ARROW: {
            return visitor_visit_list_arrow(visitor, node);
        }
        case AST_COMPOUND:
            return visitor_visit_compound(visitor, node, 1);
            break;
        case AST_NOOP:
            return node;
            break;
    }
    _GSERR_s(visitor->e, node->line, errGS201, node->type);
    _terminate_gs(visitor->e);
}
/*Tells the visitor to interpret a variable definition and store it to memory*/
AST *visitor_visit_var_def(Visitor *visitor, AST *node) {
    AST* val = visitor_visit(visitor, node->var_def_value);
    AST* def = ast_init(AST_VAR_DEF);
    def->var_def_var_name = node->var_def_var_name;
    def->var_def_value = val;
    scope_add_var_def(visitor->current_scope, def);
    return def;
}
AST *visitor_visit_var_reassign(Visitor *visitor, AST *node) {
   //  printf("\n ENCOUNTERED REASSIGN with value type %d\n", node->var_def_value->type);
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
   // printf("here");
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
    /*for(int p = 0; p < visitor->current_scope->parent->var_defs_size; p++) {
        printf(" ()in var def: %s ", visitor->current_scope->parent->var_defs[p]->var_def_var_name);
    }*/
    AST* val = scope_get_var_def(visitor->current_scope, node->var_name);
    if(val == NULL) {
        _GSERR_s(visitor->e, node->line, errGS303, node->var_name);
        return ast_init(AST_NOOP);
    } else {
        return val;
    }
}
/*Tells the visitor to interpret a function call and execute it*/
AST *visitor_visit_func_call(Visitor *visitor, AST *node, Scope *calledfrom) {
//  if(node->should_break) {printf("RETURN") ; exit(1);}
//printf("%s", node->func_call_name);
    AST *builtin = get_builtin_func(visitor, node);
    if(builtin != NULL) return builtin;
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
        if(node->func_call_args_size != fdef->func_def_args_size) {
            //printf("here name: %s, size: %d", node->func_call_name, node->func_call_args_size);
           // printf(" %s  ", ast_str[node->func_call_args[0]->type]);
            _GSERR_s(visitor->e, node->line, errGS502, node->func_call_name, node->func_call_args_size);
           // printf("\naris %s", ast_str[node->func_call_args[0]->type]);
            return ast_init(AST_NOOP);
        }
        Scope* prev = visitor->current_scope;
        if(calledfrom != NULL) {prev = calledfrom; /*printf("Calledfrom not null");*/ }
        visitor->current_scope = fdef->scope;
        //printf("%d", fdef->func_def_args_size);
        AST* res = NULL;
        AST** prev_args = calloc(fdef->func_def_args_size, sizeof(AST*));
        for(int i = 0; i < fdef->func_def_args_size; i++) {
         //   printf("\nNew Iter");
            prev_args[i] = ast_init(AST_VAR_DEF);
            prev_args[i]->var_def_value = scope_get_var_def(visitor->current_scope, fdef->func_def_args[i]->var_def_var_name);
            prev_args[i]->var_def_var_name = fdef->func_def_args[i]->var_def_var_name;
          // printf("\nhere2");
            AST* arg = ast_init(AST_VAR_DEF);
            arg->var_def_var_name = fdef->func_def_args[i]->var_def_var_name;
            visitor->current_scope = prev;
            //!! Issue is here
            arg->var_def_value = visitor_visit(visitor,node->func_call_args[i]);
            visitor->current_scope = fdef->scope;
            // printf("\n%f", arg->var_def_value->num_value);
          //   printf("\n%s", arg->var_def_var_name);
            scope_set_var(fdef->scope, arg);
        //   printf("here 3");
            // printf("%f", arg->var_def_value->num_value);
        }
       /* for(int p = 0; p < visitor->current_scope->var_defs_size; p++) {
            printf(" in func def %s ", visitor->current_scope->var_defs[p]->var_def_var_name);
        }*/
       // printf("body type: %d ", fdef->func_def_body->type);
        char *orig_file = visitor->e->curr_file;
        visitor->e->curr_file = fdef->func_def_file;
        res = visitor_visit(visitor, fdef->func_def_body);
        visitor->e->curr_file = orig_file;
        AST* returned = NULL;
        if(res->return_value) {
            char *orig_file = visitor->e->curr_file;
            visitor->e->curr_file = fdef->func_def_file;
            returned = visitor_visit(visitor, res->return_value);
            visitor->e->curr_file = orig_file;
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
    //printf("at error, file is %s", visitor->e->curr_file);
    _GSERR_s(visitor->e, node->line, errGS503, node->func_call_name);
    _terminate_gs(visitor->e);
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
                _GSERR_s(visitor->e, node->line, errGS601);
            }
            AST *result = ast_init(AST_NUM);
            result->num_value = left->num_value / right->num_value;
            return result;
        }
        case T_MOD: {
            if(right->num_value == 0) {
                _GSERR_s(visitor->e, right->line, errGS601);
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

            _GSERR_s(visitor->e, node->line, errGS604);
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
        if(cond->type != AST_BOOL) {
            _GSERR_s(visitor->e, node->line, errGS202);
            _terminate_gs(visitor->e);
        }
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
      //  printf("Starting %d iter", i+1);
        AST* stat = visitor_visit(visitor, node->repeat_body);
        //  if(scope_get_var_def(visitor->current_scope, "x")->num_value == 9) printf("ok");
        //  printf(" %f ", scope_get_var_def(visitor->current_scope, "x")->var_def_value->num_value);
     //   printf("repeated, finished %d iter\n", i+1);
        if(stat->should_break) break;
    }
 //   printf("done");
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
            AST *odef = v_node->obj_objdef;
            int c = 0;
            for(int i = 0; i < odef->fields_size; i++) {
                c++;
                if(i != 0)  strcat(buffer, ", ");
                strcat(buffer, "Field ");
                strcat(buffer, odef->fields[i]->var_def_var_name);
                strcat(buffer, ": "); // ISSUE AT LINE BELOW
                strcat(buffer, visitor_convert_to_str(visitor, scope_get_var_def(odef->scope, odef->fields[i]->var_def_var_name))->str_value);
            }
            for(int i = 0; i < odef->methods_size; i++) {
                if (c != 0) strcat(buffer, ", ");
                c++;
                strcat(buffer, "Method ");
                strcat(buffer, odef->methods[i]->func_def_name);
            }
            for(int i = 0; i < odef->objects_size; i++) {
                if(c != 0) strcat(buffer, ", ");
                c++;
                strcat(buffer, "Object ");
                strcat(buffer, odef->objects[i]->obj_name);
                strcat(buffer, ": ");
                strcat(buffer, visitor_convert_to_str(visitor, scope_get_var_def(odef->scope, odef->objects[i]->obj_name))->str_value);
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
    //Scope *objscope = scope_init(visitor->current_scope, visitor->e);
   // visitor->current_scope = objscope;
   /* for(int p = 0; p < visitor->current_scope->parent->var_defs_size; p++) {
        printf(" in obj def: %s ", visitor->current_scope->parent->var_defs[p]->var_def_var_name);
    }*/
    Scope *origin = visitor->current_scope;
    Scope *fieldscope = scope_init(NULL, visitor->e);
    AST *new_node = ast_init(AST_OBJ_DEF);
    new_node = node;
    for(unsigned i = 0; i < new_node->fields_size; i++) {
     //  printf("type: %d", node->fields[i]->var_def_value->type);
       // AST *toadd = ast_init(AST_VAR_DEF);
       // toadd = visitor_visit_var_def(visitor, new_node->fields[i]);
       // toadd->var_def_value = visitor_visit(visitor, new_node->fields[i]->var_def_value);
      //  printf(" num %d  ", node->fields[i]->var_def_value->type);
      //  toadd->var_def_var_name = new_node->fields[i]->var_def_var_name;
      //  new_node->fields[i] = toadd;
        visitor->current_scope = fieldscope;
       // scope_add_var_def(visitor->current_scope, toadd);
        visitor_visit_var_def(visitor, new_node->fields[i]);
        visitor->current_scope = origin;
      //  printf("ok2");
    }

    for(unsigned i = 0; i < new_node->methods_size; i++) {
        visitor->current_scope = fieldscope;
        visitor_visit_func_def(visitor, new_node->methods[i]);
        visitor->current_scope = origin;
    }
    for(unsigned i = 0; i < new_node->objects_size; i++) {
      //  AST *toadd = ast_init(AST_VAR_DEF);
        AST* o = ast_init(AST_OBJ);
        o->obj_objdef = visitor_visit(visitor, new_node->objects[i]);
        //o->obj_objdef = node->objects[i];
        AST* v = ast_init(AST_VAR_DEF);
        v->var_def_var_name = new_node->objects[i]->obj_name;
        v->var_def_value = o;
        visitor->current_scope = fieldscope;
        scope_add_var_def(visitor->current_scope, v);
        visitor->current_scope = origin;
    //    scope_add_var_def(visitor->current_scope, v);
    }
    new_node->scope = fieldscope;
 //   visitor->current_scope = visitor->current_scope->parent;
    AST* o = ast_init(AST_OBJ);
    o->obj_objdef = new_node;
    AST* v = ast_init(AST_VAR_DEF);
    v->var_def_var_name = new_node->obj_name;
    v->var_def_value = o;
    v->line = node->line;
    scope_add_var_def(visitor->current_scope, v);
  //  printf(" overhere ");
    return node;
}
AST *visitor_visit_objaccess(Visitor *visitor, AST *node) {
  //  printf(" visiting objacc ");
   AST *main_obj = node->objaccess_left;
   AST *v_main_obj = visitor_visit(visitor, main_obj);
   //FOR SPECIALIZED LIST OBJECT
   if(v_main_obj->type == AST_LIST) {
       // printf(" size: %d", size);
        AST *new_obj = ast_init(AST_OBJ);
        new_obj->obj_objdef = scope_get_var_def(visitor->current_scope, "_LIST_")->obj_objdef;
       // printf("%s", ast_str[v_main_obj->obj_objdef->type]);
        AST* newdef = ast_init(AST_VAR_DEF);
        newdef->var_def_var_name = "_list_";
        newdef->var_def_value = v_main_obj;
        scope_set_var(new_obj->obj_objdef->scope, newdef);
        v_main_obj = new_obj;
       // printf(" here ");
   }

   if(v_main_obj->type == AST_OBJ) {
      // printf("here");
       AST *odef = v_main_obj->obj_objdef;
       if(node->objaccess_right->type == AST_VAR) {
           char *member_name = node->objaccess_right->var_name;
             // printf(" %s ", member_name);
           AST *member = scope_get_var_def(odef->scope, (const char *) member_name);
           if (member != NULL) {
               if (node->objaccess_right->type == AST_OBJACCESS) {
                   return visitor_visit_objaccess(visitor, node->right);
               } else {
                   return member;
               }
           } else {
               _GSERR_s(visitor->e, node->line, errGS403, member_name, odef->obj_name); //!line
               return ast_init(AST_NOOP);
           }
       } else if(node->objaccess_right->type == AST_FUNC_CALL) {
           char *func_name = node->objaccess_right->func_call_name;
           AST *method = scope_get_function_definition(odef->scope, (const char*) func_name);
           if (method != NULL) {
               if (node->objaccess_right->type == AST_OBJACCESS) {
                   return visitor_visit_objaccess(visitor, node->right);
               } else {
                   Scope *origin = visitor->current_scope;
                   visitor->current_scope = odef->scope;
                   AST *res = visitor_visit_func_call(visitor, node->objaccess_right, origin);
                   visitor->current_scope = origin;
                   return res;
               }
           } else {
               _GSERR_s(visitor->e, node->line, errGS403, func_name, odef->obj_name); //!line
               return ast_init(AST_NOOP);
           }
       }
   } else {
       _GSERR_s(visitor->e, node->line, errGS404);
       return ast_init(AST_NOOP);
   }
}
AST *visitor_visit_list_arrow(Visitor *visitor, AST *node) {
//    printf("node type at start: %s\n", ast_str[node->type]);
   // printf("node left at start: %s\n", ast_str[node->list_arrow_left->type]);
   // printf("node right at start: %s\n", ast_str[node->list_arrow_right->type]);
    AST *main_list = node->list_arrow_left;
    AST *v_main_list = visitor_visit(visitor, main_list);
    if(v_main_list->type == AST_LIST) { //or maybe ast var idk
        AST* right_side = node->list_arrow_right;
        if(right_side->type != AST_LIST_ARROW) right_side = visitor_visit(visitor, node->list_arrow_right);
      //  printf("\n%s", ast_str[node->list_arrow_right->type]);
        if(right_side->type == AST_LIST_ARROW) {
           // printf(" left: %f , right: %f \n", right_side->list_arrow_left->num_value, right_side->list_arrow_right->num_value);
            AST* new_list_arrow = ast_init(AST_LIST_ARROW);
            AST* prelim_index = visitor_visit(visitor, right_side->list_arrow_left);

            if(prelim_index->type != AST_NUM) {
                _GSERR_s(visitor->e, node->line, errGS305, ast_str[prelim_index->type]);
                return ast_init(AST_NOOP);
            }
            new_list_arrow->list_arrow_left = v_main_list->list_contents[(unsigned int)prelim_index->num_value];
           // printf("ok: %s \n", ast_str[new_list_arrow->left->type]);
            new_list_arrow->list_arrow_right = right_side->list_arrow_right;
          //  printf("bruh");
           // printf("at end: %s", ast_str[new_list_arrow->list_arrow_left->type]);
            return visitor_visit_list_arrow(visitor, new_list_arrow);
        } else if(right_side->type == AST_NUM){
            if((unsigned int)right_side->num_value >= v_main_list->list_size) {
                _GSERR_s(visitor->e, right_side->line, errGS306, (int)right_side->num_value, v_main_list->list_size);
                return ast_init(AST_NOOP);
            }
            return visitor_visit(visitor, v_main_list->list_contents[(unsigned int)right_side->num_value]);
        } else {
            _GSERR_s(visitor->e, right_side->line, errGS305, ast_str[right_side->type]);
            return ast_init(AST_NOOP);
        }
    } else {
        _GSERR_s(visitor->e, node->line, errGS111, ast_str[v_main_list->type]);
        return ast_init(AST_NOOP);
    }
}
AST *visitor_visit_obj_reassign(Visitor *visitor, AST *node) {
    Scope *origin = visitor->current_scope;
    AST *val = visitor_visit(visitor, node->obj_reassign_right); //edit to see if right is valid

    char *name = visitor_scope_to_objacc(visitor, node->obj_reassign_left);

    AST* newval = ast_init(AST_VAR_DEF);
    newval->var_def_var_name = name;
    newval->var_def_value = val;

    scope_set_var(visitor->current_scope, newval);
    visitor->current_scope = origin;
    return ast_init(AST_NOOP);
}
AST *visitor_visit_list_reassign(Visitor *visitor, AST *node) {

    AST *curr = node->list_reassign_left->list_arrow_right;
    AST *v_main_list = visitor_visit(visitor, node->list_reassign_left->list_arrow_left); //check if its a list
    AST *newval = visitor_visit(visitor, node->list_reassign_right);
    //printf(" TYPE OF LIST ARR RIGHT: %s, %f\n", ast_str[curr->type], curr->num_value);
    //printf(" TYPE OF LIST ARR LEFT: %s, %f\n", ast_str[v_main_list->type], v_main_list->num_value);
    while(curr->type == AST_LIST_ARROW) {
        //printf(" in loop ");
        AST* new_list_arrow = ast_init(AST_LIST_ARROW);
        AST* prelim_index = visitor_visit(visitor, curr->list_arrow_left);

        if(prelim_index->type != AST_NUM) {
            _GSERR_s(visitor->e, node->line, errGS305, ast_str[prelim_index->type]);
            return ast_init(AST_NOOP);
        }
        new_list_arrow->list_arrow_left = v_main_list->list_contents[(unsigned int)prelim_index->num_value];
        // printf("ok: %s \n", ast_str[new_list_arrow->left->type]);
        new_list_arrow->list_arrow_right = curr->list_arrow_right;
        curr = new_list_arrow;
        if(curr->list_arrow_right->type != AST_LIST_ARROW) {
            AST *f_index = visitor_visit(visitor, curr->list_arrow_right);
            if(f_index->type != AST_NUM) {
                _GSERR_s(visitor->e, f_index->line, errGS305, ast_str[f_index->type]);
                return ast_init(AST_NOOP);
            }
            printf("%f",f_index->num_value);
            curr->list_arrow_left->list_contents[(unsigned int)f_index->num_value] = newval;
            return ast_init(AST_NOOP);
        }
    }
    //printf("here");
    //printf("  %s  ", ast_str[node->list_reassign_left->list_arrow_left->type]);
    curr = visitor_visit(visitor, curr);
    if(curr->type != AST_NUM) {
        _GSERR_s(visitor->e, curr->line, errGS305, ast_str[curr->type]);
        return ast_init(AST_NOOP);
    }
    AST* list = visitor_visit(visitor, node->list_reassign_left->list_arrow_left);
    list->list_contents[(unsigned int)curr->num_value] = newval;
    //add way to change value of list
    return ast_init(AST_NOOP);

}
char *visitor_scope_to_objacc(Visitor *visitor, AST *node) {
    AST *main_obj = node->objaccess_left;
    AST *v_main_obj = visitor_visit(visitor, main_obj);
    if(v_main_obj->type == AST_OBJ) {
        // printf("here");
        AST *odef = v_main_obj->obj_objdef;
        if (node->objaccess_right->type == AST_VAR) {
            char *member_name = node->objaccess_right->var_name;
            // printf(" %s ", member_name);
            AST *member = scope_get_var_def(odef->scope, (const char *) member_name);
            if (member != NULL) {
                visitor->current_scope = odef->scope;
                if (node->objaccess_right->type == AST_OBJACCESS) {
                    visitor_scope_to_objacc(visitor, node->right);
                } else {
                    // printf("%d", member->type);
                    return member_name;
                }
            } else {
                _GSERR_s(visitor->e, node->line, errGS403, member_name, odef->obj_name); //!line
                return ast_init(AST_NOOP);
            }
        } else {
            //make into actual error
            printf("illegal for obj reassignment");
        }
    }
}
AST *visitor_visit_import(Visitor *visitor, AST *node) {
    char * filepath = calloc(100, sizeof(char));
    filepath[99] = '\0';
    if(strcmp(node->import_val, "entry.gs") == 0) {
        _GSERR_s(visitor->e, node->line, errGS109);
        return ast_init(AST_NOOP);
    }
    if(strcmp(node->import_val, visitor->e->curr_file) == 0) {
        _GSERR_s(visitor->e, node->line, errGS110, node->import_val, node->import_val);
        return ast_init(AST_NOOP);
    }
    if(node->import_is_builtin) {
        strcat(filepath, "../gs-libs/");
        strcat(filepath, node->import_val);
    } else {
        strcat(filepath, "../gs-entry/");
        strcat(filepath, node->import_val);
    }
    Lexer *l = lexer_init(get_file_data(filepath));
    char * origin = visitor->e->curr_file;
    l->e = visitor->e;
    l->e->curr_file = node->import_val;
    Parser *p = parser_init(l);
    AST* root = parser_parse(p, p->scope);
    visitor_visit_compound(visitor, root, 0);
    visitor->e->curr_file = origin;
    free(p);
    free(l);
    return ast_init(AST_NOOP);
}
AST *visitor_visit_compound(Visitor *visitor, AST *node, unsigned change_scope) {
    Scope* compound_scope = scope_init(visitor->current_scope, visitor->e);
    if(change_scope) {
        visitor->current_scope = compound_scope;
    }
    for (int i = 0; i < node->compound_size; i++) {
        //printf(" in compound: %d ", node->compound_value[i]->type);
       /* for(int p = 0; p < visitor->current_scope->parent->var_defs_size; p++) {
            printf(" (%d)in comp def: %s ",i, visitor->current_scope->parent->var_defs[p]->var_def_var_name);
        }*/

        AST* stat = visitor_visit(visitor, node->compound_value[i]);
        if(stat->should_return || stat->should_break) return stat;
    }
  //  printf("done with comp");
  if(change_scope) {
      visitor->current_scope = compound_scope->parent;
  }

  //  free(compound_scope);
    return node;
}