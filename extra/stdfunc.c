#include "stdfunc.h"
#include "../core/visitor.h"
char* ast_str_1[29] = { "variable definition", "function definition", "variable", "function call", "string", "compound statements", "no operation", "number", "binary operation", "boolean", "if statement", "repeat statement", "out statement", "skip statement", "variable reassignment", "while statement", "unary operation", "parameter", "return statement", "list", "list index", "object reference", "group", "object definition", "dot", "object access", "object", "import statement", "object reassignment"};
AST *std_func_write(Visitor *visitor, AST **argv, int argc, unsigned line) {
    for (int i = 0; i < argc; i++) {
        AST *visited_ast = visitor_visit(visitor, argv[i]);
        if (visited_ast->type != AST_STR && visited_ast->type != AST_NUM &&
            visited_ast->type != AST_BOOL && visited_ast->type != AST_LIST &&
            visited_ast->type != AST_LISTINDX && visited_ast->type != AST_OBJ) {
            // printast(visited_ast->type);
            //_GSERR(line, "GS504 - Illegal argument to function 'write'");
            _GSERR_s(visitor->e, line,
                     errGS504, "write", ast_str_1[visited_ast->type]);
            return ast_init(AST_NOOP);
            //_terminate_gs(visitor->e);
        }
        printf("%s", visitor_convert_to_str(visitor, visited_ast)->str_value);
    }
    printf("\n");
    // AST* a = scope_get_function_definition(visitor->current_scope, "recur");
    // printf("%lf\n", a->func_def_args[0]->num_value);
    return ast_init(AST_NOOP);
}
AST *std_func_read(Visitor *visitor, AST **argv, int argc, unsigned line) {
    if (argc > 1) {
        _GSERR_s(visitor->e, line, "GS502 - Function 'read' does not take %d arguments", argc);
        // exit(1);
        return ast_init(AST_NOOP);
    } else {
        AST *visited_ast = visitor_visit(visitor, argv[0]);
        switch (visited_ast->type) {
            case AST_STR: {
                printf("%s", visited_ast->str_value);
                char *result = malloc(sizeof(char *));
                fgets(result, 1000, stdin);
                if ((strlen(result) > 0) && (result[strlen(result) - 1] == '\n'))
                    result[strlen(result) - 1] = '\0';
                AST *uinput_ast = ast_init(AST_STR);
                uinput_ast->str_value = result;
                return uinput_ast;
            }
            default: {
                _GSERR_s(visitor->e, line, "GS504 - Illegal argument to function 'read'");
                break;
            }
        }
    }
}
AST *std_func_toupper(Visitor *visitor, AST **argv, int argc, unsigned line) {
    if ((argv[0]->str_value == NULL && argv[0]->func_call_name == NULL &&
         argv[0]->var_name == NULL && argv[0]->type != AST_NUM) ||
        argv[0]->type == AST_NOOP) {
        _GSERR_s(visitor->e, line,
                 "GS501 - No arguments provided where needed for function 'toupper'");
        return ast_init(AST_NOOP);
    }
    if (argc != 1) {
        _GSERR_s(visitor->e, line, "GS502 - Function 'toupper' does not take %d arguments", argc);
        return ast_init(AST_NOOP);
    } else {
        AST *visited_ast = visitor_visit(visitor, argv[0]);
        switch (visited_ast->type) {
            case AST_STR: {
                AST *upper_ast = ast_init(AST_STR);
                // char result[strlen(visited_ast->str_value)];

                for (int i = 0; i < strlen(visited_ast->str_value); i++) {
                    if (isalpha(visited_ast->str_value[i])) {
                        visited_ast->str_value[i] = toupper(visited_ast->str_value[i]);
                    } else {
                        visited_ast->str_value[i] = visited_ast->str_value[i];
                    }
                }
                upper_ast->str_value = visited_ast->str_value;
                return upper_ast;
            }
            default: {
                _GSERR_s(visitor->e, line, "GS504 - Illegal argument to function 'toupper'");
                return ast_init(AST_NOOP);
            }
        }
    }
}
AST *std_func_tolower(Visitor *visitor, AST **argv, int argc, unsigned line) {
    if (argv[0]->str_value == NULL && argv[0]->func_call_name == NULL &&
        argv[0]->var_name == NULL && argv[0]->type != AST_NUM) {
        _GSERR(line,
               "GS501 - No arguments provided where needed for function 'tolower'");
        return ast_init(AST_NOOP);
    }
    if (argc != 1) {
        char d[100];
        sprintf(d, "GS502 - Function 'tolower' does not take %d arguments", argc);
        _GSERR(line, d);
        return ast_init(AST_NOOP);
    } else {
        AST *visited_ast = visitor_visit(visitor, argv[0]);
        // printf("%d", visited_ast->type);
        switch (visited_ast->type) {
            case AST_STR: {
                AST *lower_ast = ast_init(AST_STR);
                for (int i = 0; i < strlen(visited_ast->str_value); i++) {
                    if (isalpha(visited_ast->str_value[i])) {
                        visited_ast->str_value[i] = tolower(visited_ast->str_value[i]);
                    } else {
                        visited_ast->str_value[i] = visited_ast->str_value[i];
                    }
                }
                lower_ast->str_value = visited_ast->str_value;
                return lower_ast;
            }
            default: {
                _GSERR(line, "GS504 - Illegal argument to function 'tolower'");
                break;
            }
        }
    }
}
AST *std_func_randnum(Visitor *visitor, AST **argv, int argc, unsigned line) {
    AST *result = ast_init(AST_NUM);
    result->num_value = rand();
    return result;
}
AST *std_func_num(Visitor *visitor, AST **argv, int argc, unsigned line) {
    if (argv[0]->str_value == NULL && argv[0]->func_call_name == NULL &&
        argv[0]->var_name == NULL && argv[0]->type != AST_NUM) {
        _GSERR(line,
               "GS501 - No arguments provided where needed for function 'num'");
        return ast_init(AST_NOOP);
    }
    if (argc != 1) {
        char d[100];
        sprintf(d, "GS502 - Function 'num' does not take %d arguments", argc);
        _GSERR(line, d);
        return ast_init(AST_NOOP);
    } else {
        AST *visited_ast = visitor_visit(visitor, argv[0]);
        if (visited_ast->type != AST_STR)
            _GSERR(line, "GS504 - Illegal argument to function 'num'");
        AST *result = ast_init(AST_NUM);
        result->num_value = strtof(visited_ast->str_value, NULL);
        return result;
    }
}
AST *std_func_len(Visitor *visitor, AST **argv, int argc, unsigned line) {
    if (argc != 1) {
        char d[100];
        sprintf(d, "GS502 - Function 'len' does not take %d arguments", argc);
        _GSERR(line, d);
    }
    AST *target = visitor_visit(visitor, argv[0]);
    if (target->type != AST_STR) {
        _GSERR(line, "GS504 - Illegal argument to function 'len'");
    }
    AST *len_ast = ast_init(AST_NUM);
    len_ast->num_value = strlen(target->str_value);
    len_ast->line = line;
    return len_ast;
}
AST *std_func_strindx(Visitor *visitor, AST **argv, int argc, unsigned line) {
    if (argc != 2) {
        char d[100];
        sprintf(d, "GS502 - Function 'strindx' does not take %d arguments", argc);
        _GSERR(line, d);
    }
    AST *ref = visitor_visit(visitor, argv[0]);
    if (ref->type != AST_STR) {
        _GSERR(line, "GS504 - Illegal argument to function 'strindx'");
    }
    AST *index = visitor_visit(visitor, argv[1]);
    if (index->type != AST_NUM) {
        _GSERR(line, "GS504 - Illegal argument to function 'strindx'");
    }
    AST *char_ast = ast_init(AST_STR);
    char_ast->str_value = malloc(2 * sizeof(char));
    char_ast->str_value[0] = ref->str_value[(unsigned)index->num_value];
    char_ast->str_value[1] = '\0';
    char_ast->line = line;
    return char_ast;
}
AST *std_func_listindx(Visitor *visitor, AST **argv, int argc, unsigned line) {
    if(argc != 2) {
        _GSERR_s(visitor->e, line, "GS502 - Function 'listindx' does not take %d arguments", argc);
        return ast_init(AST_NOOP);
    }
    AST *list_var = visitor_visit(visitor, argv[0]);
    if(list_var->type != AST_LIST) {
        _GSERR_s(visitor->e, line, "GS504 - Illegal argument to function 'listindx'");
        return ast_init(AST_NOOP);
    }
    AST *index = visitor_visit(visitor, argv[1]);
    if(index->type != AST_NUM) {
        _GSERR_s(visitor->e, line, "GS504 - Illegal argument to function 'listindx'");
        return ast_init(AST_NOOP);
    }
    return list_var->list_contents[(unsigned)index->num_value];
}
AST *get_builtin_func(Visitor *visitor, AST *node) {
    int argc = node->func_call_args_size;
    AST ** argv = node->func_call_args;
    //printf(" %s %s", node->func_call_name, visitor->e->curr_file);
    if(strcmp(node->func_call_name, "__builtin_sin__") == 0 && strcmp(visitor->e->curr_file, "builtin_math.gs") == 0) {
        AST * ast = ast_init(AST_NUM);
        AST * v = visitor_visit(visitor, argv[0]);
        ast->num_value = sin(v->num_value);
        return ast;
    } else if(strcmp(node->func_call_name, "__builtin_cos__") == 0 && strcmp(visitor->e->curr_file, "builtin_math.gs") == 0) {
        AST * ast = ast_init(AST_NUM);
        AST * v = visitor_visit(visitor, argv[0]);
        ast->num_value = cos(v->num_value);
        return ast;
    } else if(strcmp(node->func_call_name, "__builtin_tan__") == 0 && strcmp(visitor->e->curr_file, "builtin_math.gs") == 0) {
        AST * ast = ast_init(AST_NUM);
        AST * v = visitor_visit(visitor, argv[0]);
        ast->num_value = tan(v->num_value);
        return ast;
    } else if(strcmp(node->func_call_name, "__builtin_sqrt__") == 0 && strcmp(visitor->e->curr_file, "builtin_math.gs") == 0) {
        AST * ast = ast_init(AST_NUM);
        AST * v = visitor_visit(visitor, argv[0]);
        ast->num_value = sqrt(v->num_value);
        return ast;
    } else if(strcmp(node->func_call_name, "__builtin_root__") == 0 && strcmp(visitor->e->curr_file, "builtin_math.gs") == 0) {
        AST * ast = ast_init(AST_NUM);
        AST * v = visitor_visit(visitor, argv[0]);
        AST * v2 = visitor_visit(visitor, argv[1]);
        ast->num_value = round(pow((int)(v->num_value), (1.0 / (int)(v2->num_value))));
        return ast;
    }  else if(strcmp(node->func_call_name, "__builtin_pow__") == 0 && strcmp(visitor->e->curr_file, "builtin_math.gs") == 0) {
        AST *ast = ast_init(AST_NUM);
        AST *v = visitor_visit(visitor, argv[0]);
        AST *v2 = visitor_visit(visitor, argv[1]);
        ast->num_value = pow(v->num_value, v2->num_value);
        return ast;
    } else return NULL;
}