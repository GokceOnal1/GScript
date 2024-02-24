#include "parser.h"


/* Functions for parsing input */
char* toks[32] = { "identifier", "=", "string", ";", "(", ")", "{", "}", "$", ",", "end of file", "number", "+", "-", "*", "/", "<", ">", "==", "<=", ">=", "!=", "&", "|", "%", "!", "[", "]", ":", "~", ".", "macro expression", "->" };

/*init for parser*/
Parser *parser_init(Lexer *lexer) {
    Parser *parser = calloc(1, sizeof(Parser));
    parser->lexer = lexer;
    parser->current_token = lexer_get_next_token(lexer);
    parser->previous_token = parser->current_token;
    parser->scope = scope_init(NULL, lexer->e);
    parser->e = lexer->e;
    return parser;
}
/*tells parser what type to expect for current token*/
void parser_eat(Parser *parser, int token_type) {
    if (parser->current_token->type == token_type) {
        parser->previous_token = parser->current_token;
        parser->current_token = lexer_get_next_token(parser->lexer);
    } else {
        _GSERR_s(parser->e, parser->current_token->line,
                 errGS101,
                 toks[token_type], parser->current_token->value,
                 toks[parser->current_token->type]);
        _terminate_gs(parser->e);
        // exit(1);
    }
}
/*short for statements*/
AST *parser_parse(Parser *parser, Scope *scope) {
    return parser_parse_statements(parser, scope);
}
/*Parses next statement from the lexer*/
AST *parser_parse_statement(Parser *parser, Scope *scope) {
    switch (parser->current_token->type) {
        case T_ID:
            return parser_parse_id(parser, scope);
        case T_STR:
            return parser_parse_string(parser, scope);
        default:
            break;
    }
    return ast_init(AST_NOOP);
}
/*Parses next multiple statements from the lexer and returns AST Node with
 * compound of statements*/
AST *parser_parse_statements(Parser *parser, Scope *scope) {
    AST *compound = ast_init(AST_COMPOUND);
    compound->compound_value = calloc(1, sizeof(AST **));
    compound->scope = scope;

    AST *ast_statement = parser_parse_comp_expr(parser, scope);
    ast_statement->scope = scope;
    compound->compound_value[0] = ast_statement;
    compound->compound_size++;

    while (parser->current_token->type == T_SEMI) {
        parser_eat(parser, T_SEMI);

        AST *ast = parser_parse_comp_expr(parser, scope);
        if (ast) {
            compound->compound_size++;
            AST** tmp = realloc(
                    compound->compound_value, compound->compound_size * sizeof(AST *));
            if(tmp) compound->compound_value = tmp; else {
                _GSERR_s(parser->e, parser->current_token->line, errGSb02);
                _terminate_gs(parser->e);
            }
            compound->compound_value[compound->compound_size - 1] = ast;
        }
    }
    return compound;
}
/*Parses an expression from the lexer and returns AST Node with value of
 * expression*/
AST *parser_parse_expr(Parser *parser, Scope *scope) {
    /*switch (parser->current_token->type) {
    case T_STR:
      return parser_parse_string(parser, scope);
    case T_ID:
      return parser_parse_id(parser, scope);
    }
    return ast_init(AST_NOOP);*/
    AST *ast_left = parser_parse_term(parser, scope);
    while (parser->current_token->type == T_PLUS ||
           parser->current_token->type == T_MIN) {
        AST *ast_binop = ast_init(AST_BINOP);
        ast_binop->left = ast_left;
        ast_binop->op = parser->current_token->type;
        ast_binop->line = parser->current_token->line;
        parser_eat(parser, parser->current_token->type);
        ast_binop->right = parser_parse_term(parser, scope);
        ast_left = ast_binop;
    }
    return ast_left;
}
AST *parser_parse_comp_expr(Parser *parser, Scope *scope) {
    AST *ast_left = parser_parse_comp_term(parser, scope);
    while (parser->current_token->type == T_AND ||
           parser->current_token->type == T_OR) {
        AST *ast_binop = ast_init(AST_BINOP);
        ast_binop->left = ast_left;
        ast_binop->op = parser->current_token->type;
        ast_binop->line = parser->current_token->line;
        parser_eat(parser, parser->current_token->type);
        ast_binop->right = parser_parse_comp_term(parser, scope);
        ast_left = ast_binop;
    }
    return ast_left;
}
AST *parser_parse_comp_term(Parser *parser, Scope *scope) {
    AST *ast_left = parser_parse_expr(parser, scope);
    while (parser->current_token->type == T_EE ||
           parser->current_token->type == T_LT ||
           parser->current_token->type == T_LTE ||
           parser->current_token->type == T_GT ||
           parser->current_token->type == T_GTE ||
           parser->current_token->type == T_NEQ) {
        AST *ast_binop = ast_init(AST_BINOP);
        ast_binop->left = ast_left;
        ast_binop->op = parser->current_token->type;
        ast_binop->line = parser->current_token->line;
        parser_eat(parser, parser->current_token->type);
        ast_binop->right = parser_parse_expr(parser, scope);
        ast_left = ast_binop;
    }
    return ast_left;
}
AST *parser_parse_factor(Parser *parser, Scope *scope) {
    AST *ast_left = parser_parse_obj_access(parser, scope);

    return ast_left;
}
AST *parser_parse_mono(Parser *parser, Scope *scope) {
    switch (parser->current_token->type) {
        case T_ID:
            return parser_parse_id(parser, scope);
        case T_NUM:
            return parser_parse_num(parser, scope);
        case T_STR:
            return parser_parse_string(parser, scope);
        case T_LPR: {
            parser_eat(parser, T_LPR);
            AST *ast = parser_parse_comp_expr(parser, scope);
            parser_eat(parser, T_RPR);
            return ast;
        }
        case T_LSQB: {
            AST *ast = parser_parse_list_body(parser, scope);
            return ast;
        }
        case T_MIN: {
            if (parser->previous_token->type == T_MIN) {
            }
            parser_eat(parser, T_MIN);
            AST *ast = ast_init(AST_UNOP);
            ast->unop_op = T_MIN;
            ast->unop_body = parser_parse_factor(parser, scope);
            return ast;
        }
        case T_NOT: {
            parser_eat(parser, T_NOT);
            AST *ast = ast_init(AST_UNOP);
            ast->unop_op = T_NOT;
            ast->unop_body = parser_parse_factor(parser, scope);
            return ast;
        }
        default: {
            return ast_init(AST_NOOP);
        }
    }
}
AST *parser_parse_term(Parser *parser, Scope *scope) {
    AST *ast_left = parser_parse_factor(parser, scope);
    while (parser->current_token->type == T_MUL ||
           parser->current_token->type == T_DIV ||
           parser->current_token->type == T_MOD) {
        AST *ast_binop = ast_init(AST_BINOP);
        ast_binop->left = ast_left;
        ast_binop->op = parser->current_token->type;
        ast_binop->line = parser->current_token->line;
        parser_eat(parser, parser->current_token->type);
        ast_binop->right = parser_parse_factor(parser, scope);
        ast_left = ast_binop;
    }
    return ast_left;
}
/*Parses a function call from the lexer and returns AST Node with relevant
 * information about the function*/
AST *parser_parse_function_call(Parser *parser, Scope *scope) {
    AST *function_call = ast_init(AST_FUNC_CALL);
    function_call->line = parser->previous_token->line;
    function_call->func_call_name = parser->previous_token->value;
    parser_eat(parser, T_LPR);
    function_call->func_call_args = calloc(1, sizeof(AST **));

    AST *ast_expr = parser_parse_comp_expr(parser, scope);
    function_call->func_call_args[0] = ast_expr;
    function_call->func_call_args_size++;

    while (parser->current_token->type == T_COMMA) {
        parser_eat(parser, T_COMMA);
        AST *ast = parser_parse_comp_expr(parser, scope);
        function_call->func_call_args_size++;
        AST** tmp = realloc(function_call->func_call_args,
                            function_call->func_call_args_size * sizeof(AST *));
        if(tmp) function_call->func_call_args = tmp; else {
            _GSERR_s(parser->e, parser->current_token->line, errGSb02);
            _terminate_gs(parser->e);
        }
        function_call->func_call_args[function_call->func_call_args_size - 1] =
                ast;
    }
    parser_eat(parser, T_RPR);
    function_call->scope = scope;
    return function_call;
}
/*Parses a variable reference from the lexer and returns AST Node with relevant
 * information*/
AST *parser_parse_variable(Parser *parser, Scope *scope) {
    char *token_value = parser->current_token->value;
    parser_eat(parser, T_ID); // Either var name, or func call name, or obj name
    if (parser->current_token->type == T_LPR) {
        return parser_parse_function_call(parser, scope);
    } else if (parser->current_token->type == T_EQL) {
        return parser_parse_var_reassign(parser, scope);
    } else if (parser->current_token->type == T_TILDE) {
        AST *ref = ast_init(AST_OBJREF);
        ref->objref_name = parser->previous_token->value;
        parser_eat(parser, T_TILDE);
        return ref;
    }
    AST *ast_var = ast_init(AST_VAR);
    ast_var->line = parser->previous_token->line;
    ast_var->var_name = token_value;
    ast_var->scope = scope;
    return ast_var;
}
/*Parses a string from the lexer and returns AST Node with relevant
 * information*/
AST *parser_parse_string(Parser *parser, Scope *scope) {
    AST *ast_str = ast_init(AST_STR);
    ast_str->str_value = parser->current_token->value;
    ast_str->line = parser->current_token->line;
    parser_eat(parser, T_STR);
    ast_str->scope = scope;
    return ast_str;
}
/*Parses an identifier from the lexer and returns AST Node with relevant
 * information*/
AST *parser_parse_id(Parser *parser, Scope *scope) {
    if (strcmp(parser->current_token->value, "assign") == 0) {
        return parser_parse_variable_definition(parser, scope);
    } else if (strcmp(parser->current_token->value, "funct") == 0) {
        return parser_parse_function_definition(parser, scope);
    } else if (strcmp(parser->current_token->value, "if") == 0) {
        return parser_parse_if_statement(parser, scope);
    } else if (strcmp(parser->current_token->value, "repeat") == 0) {
        return parser_parse_repeat_statement(parser, scope);
    } else if (strcmp(parser->current_token->value, "leave") == 0) {
        return parser_parse_leave_statement(parser, scope);
    } else if (strcmp(parser->current_token->value, "return") == 0) {
        return parser_parse_return_statement(parser, scope);
    } else if (strcmp(parser->current_token->value, "while") == 0) {
        return parser_parse_while_statement(parser, scope);
    } else if (strcmp(parser->current_token->value, "elsif") == 0) {
        _GSERR_s(parser->e, parser->current_token->line, errGS105);
        return NULL;
    } else if (strcmp(parser->current_token->value, "else") == 0) {
        _GSERR_s(parser->e, parser->current_token->line, errGS106);
        return NULL;
    } else if (strcmp(parser->current_token->value, "obj") == 0) {
        return parser_parse_obj(parser, scope);
    } else if(strcmp(parser->current_token->value, "import") == 0) {
        return parser_parse_import(parser, scope);
    } else if ((strcmp(parser->current_token->value, "true") == 0) ||
               (strcmp(parser->current_token->value, "false") == 0)) {

        AST *result = ast_init(AST_BOOL);
        result->b_value = strcmp(parser->current_token->value, "true") == 0 ? 1 : 0;
        result->line = parser->current_token->line;
        parser_eat(parser, T_ID);
        //  printf("%s", parser->current_token->value);
        //  printf("%i", result->b_value);
        return result;
    } else {
        return parser_parse_variable(parser, scope);
    }
}
AST *parser_parse_num(Parser *parser, Scope *scope) {
   // printf("%s\n", parser->current_token->value);
    double value = strtod(parser->current_token->value, NULL);
   // printf("%lf\n", value);
    parser_eat(parser, T_NUM);
    AST *ast = ast_init(AST_NUM);
    ast->num_value = value;
    ast->line = parser->current_token->line;
    return ast;
}
AST *parser_parse_var_reassign(Parser *parser, Scope *scope) {
    // printf("reassigning");
    AST *newdef = ast_init(AST_REASSIGN);
    newdef->var_def_var_name = parser->previous_token->value;
    newdef->line = parser->previous_token->line;
    parser_eat(parser, T_EQL);
    newdef->var_def_value = parser_parse_comp_expr(parser, scope);
    return newdef;
}
/*Parses a variable definition from the lexer and returns AST Node with relevant
 * information*/
AST *parser_parse_variable_definition(Parser *parser, Scope *scope) {
    parser_eat(parser, T_ID);
    char *var_name = parser->current_token->value;
    unsigned int l = parser->current_token->line;
    parser_eat(parser, T_ID);
    parser_eat(parser, T_EQL);
    AST *var_value = parser_parse_comp_expr(parser, scope);
    AST *var_def = ast_init(AST_VAR_DEF);
    var_def->var_def_var_name = var_name;
    var_def->line = l;
    var_def->var_def_value = var_value;
    var_def->scope = scope;
    return var_def;
}
/*Parses a function defition from the lexer and returns AST Node with relevant
 * information*/
AST *parser_parse_function_definition(Parser *parser, Scope *scope) {
    AST *ast = ast_init(AST_FUNC_DEF);
    ast->func_def_file = parser->e->curr_file;
    parser_eat(parser, T_ID);
    char *func_name = parser->current_token->value;
    ast->line = parser->current_token->line;
    ast->func_def_name = calloc(strlen(func_name) + 1, sizeof(char));
    strcpy(ast->func_def_name, func_name);
    parser_eat(parser, T_ID);
    parser_eat(parser, T_LPR);
    // Parses parameters of a function definition
    if (parser->current_token->type != T_RPR) {
     //   printf("iter");
        ast->func_def_args = calloc(1, sizeof(AST *));
        AST *arg = parser_parse_function_param(parser, scope);
        ast->func_def_args_size++;
        ast->func_def_args[ast->func_def_args_size - 1] = arg;
        // scope_add_var_def(scope, arg);
    }

    while (parser->current_token->type == T_COMMA) {
        parser_eat(parser, T_COMMA);
     //   printf("iter");
        ast->func_def_args_size++;
        ast->func_def_args =
                realloc(ast->func_def_args, ast->func_def_args_size * sizeof(AST *));
        AST *arg = parser_parse_function_param(parser, scope);
        ast->func_def_args[ast->func_def_args_size - 1] = arg;
        // scope_add_var_def(scope, arg);
    }

    parser_eat(parser, T_RPR);
    parser_eat(parser, T_LBR);
    ast->func_def_body = parser_parse_statements(parser, scope);
    parser_eat(parser, T_RBR);
    ast->scope = scope;
    ast->line = parser->current_token->line;
    return ast;
}
AST *parser_parse_if_statement(Parser *parser, Scope *scope) {
    AST *ast = ast_init(AST_IF);
    parser_eat(parser, T_ID);
    parser_eat(parser, T_LPR);
    ast->if_conds = calloc(1, sizeof(AST *));
    ast->if_conds[0] = parser_parse_comp_expr(parser, scope);
    parser_eat(parser, T_RPR);
    parser_eat(parser, T_LBR);
    ast->if_bodies = calloc(1, sizeof(AST *));
    ast->if_bodies[0] = parser_parse_statements(parser, scope);
    parser_eat(parser, T_RBR);
    ast->if_bodies_size++;
    ast->if_conds_size++;
    while (parser->current_token->type == T_ID &&
           strcmp(parser->current_token->value, "elsif") == 0) {
        parser_eat(parser, T_ID);
        parser_eat(parser, T_LPR);
        ast->if_bodies_size++;
        ast->if_conds_size++;
        ast->if_conds =
                (AST **)realloc(ast->if_conds, ast->if_conds_size * sizeof(AST *));
        ast->if_conds[ast->if_conds_size - 1] =
                parser_parse_comp_expr(parser, scope);
        parser_eat(parser, T_RPR);
        parser_eat(parser, T_LBR);
        ast->if_bodies =
                (AST **)realloc(ast->if_bodies, ast->if_bodies_size * sizeof(AST *));
        ast->if_bodies[ast->if_bodies_size - 1] =
                parser_parse_statements(parser, scope);
        parser_eat(parser, T_RBR);
    }
    if (parser->current_token->type == T_ID &&
        strcmp(parser->current_token->value, "else") == 0) {
        parser_eat(parser, T_ID);
        parser_eat(parser, T_LBR);
        ast->else_body = parser_parse_statements(parser, scope);
        parser_eat(parser, T_RBR);
    }
    ast->scope = scope;
    ast->line = parser->current_token->line;
    return ast;
}
AST *parser_parse_repeat_statement(Parser *parser, Scope *scope) {
    AST *ast = ast_init(AST_REPEAT);
    parser_eat(parser, T_ID);
    parser_eat(parser, T_LPR);
    ast->repeat_count = parser_parse_expr(parser, scope);
    parser_eat(parser, T_RPR);
    parser_eat(parser, T_LBR);
    ast->repeat_body = parser_parse_statements(parser, scope);
    parser_eat(parser, T_RBR);
    ast->scope = scope;
    ast->line = parser->current_token->line;
    return ast;
}
AST *parser_parse_leave_statement(Parser *parser, Scope *scope) {
    AST *ast = ast_init(AST_OUT);
    parser_eat(parser, T_ID);
    ast->line = parser->current_token->line;
    return ast;
}
AST *parser_parse_return_statement(Parser *parser, Scope *scope) {
    AST *ast = ast_init(AST_RETURN);
    parser_eat(parser, T_ID);
    ast->should_return = 1;
    ast->return_value = parser_parse_comp_expr(parser, scope);
    ast->line = parser->current_token->line;
    return ast;
}
AST *parser_parse_while_statement(Parser *parser, Scope *scope) {
    AST *ast = ast_init(AST_WHILE);
    parser_eat(parser, T_ID);
    parser_eat(parser, T_LPR);
    ast->while_cond = parser_parse_comp_expr(parser, scope);
    parser_eat(parser, T_RPR);
    parser_eat(parser, T_LBR);
    ast->while_body = parser_parse_statements(parser, scope);
    parser_eat(parser, T_RBR);
    ast->scope = scope;
    ast->line = parser->current_token->line;
    return ast;
}
AST *parser_parse_function_param(Parser *parser, Scope *scope) {
    AST *ast = ast_init(AST_VAR_DEF);
    if (strcmp(parser->current_token->value, "param") != 0) {
        _GSERR_s(parser->e, parser->current_token->line,
                 errGS506);
    }
    parser_eat(parser, T_ID);
    ast->var_def_var_name = parser->current_token->value;
    parser_eat(parser, T_ID);
    ast->var_def_value = ast_init(AST_NUM);
    ast->line = parser->current_token->line;
    return ast;
}
AST *parser_parse_list_body(Parser *parser, Scope *scope) {
    AST *ast = ast_init(AST_LIST);
    parser_eat(parser, T_LSQB);
    ast->list_contents = calloc(1, sizeof(AST *));
    ast->list_contents[0] = parser_parse_comp_expr(parser, scope);
    ast->list_size++;
    while (parser->current_token->type == T_COMMA) {
        parser_eat(parser, T_COMMA);
        ast->list_size++;
        ast->list_contents =
                realloc(ast->list_contents, ast->list_size * sizeof(AST *));
        ast->list_contents[ast->list_size - 1] =
                parser_parse_comp_expr(parser, scope);
    }
    parser_eat(parser, T_RSQB);
    ast->line = parser->current_token->line;
    return ast;
}
AST *parser_parse_list_sub_index(Parser *parser, Scope *scope) {
    AST *ast = ast_init(AST_LISTINDX);
    // ast->list_var_name = parser->previous_token->value;
    ast->line = parser->previous_token->line;
    parser_eat(parser, T_LSQB);
    ast->list_sub_index = (unsigned)parser_parse_expr(parser, scope)->num_value;
    parser_eat(parser, T_RSQB);
    return ast;
}
AST *parser_parse_obj(Parser *parser, Scope *scope) {
    AST *ast = ast_init(AST_OBJ_DEF);
    parser_eat(parser, T_ID);
    ast->obj_name = parser->current_token->value;
    parser_eat(parser, T_ID);
    parser_eat(parser, T_EQL);
    parser_eat(parser, T_LBR);
    while(parser->current_token->type != T_RBR){
        AST *res = parser_parse_field_or_method(parser, scope);
        if(res->type == AST_VAR_DEF) {
            if(ast->fields_size == 0) {
                ast->fields_size = 1;
                ast->fields = calloc(1, sizeof(AST*));
                ast->fields[0] = res;
            //    printf("%s", res->var_def_var_name);
            } else {
                ast->fields_size++;
                AST **temp = (AST**)realloc(ast->fields, ast->fields_size * sizeof(AST*));
                if(temp) {
                    ast->fields = temp;
                } else {
                    _GSERR_s(parser->e, parser->current_token->line,errGSb02);
                }
                ast->fields[ast->fields_size-1] = res;
            }
        } else if(res->type == AST_FUNC_DEF) {
            if(ast->methods_size == 0) {
                ast->methods_size = 1;
                ast->methods = calloc(1, sizeof(AST*));
                ast->methods[0] = res;
            } else {
                ast->methods_size++;
                AST **temp = (AST**)realloc(ast->methods, ast->methods_size * sizeof(AST*));
                if(temp) {
                    ast->methods = temp;
                } else {
                    _GSERR_s(parser->e, parser->current_token->line,errGSb02);
                }
                ast->methods[ast->methods_size-1] = res;
            }
        } else if(res->type == AST_OBJ_DEF) {
            if(ast->objects_size == 0) {
                ast->objects_size = 1;
                ast->objects = calloc(1, sizeof(AST*));
                ast->objects[0] = res;
            } else {
                ast->objects_size++;
                AST **temp = (AST**)realloc(ast->objects, ast->objects_size * sizeof(AST*));
                if(temp) {
                    ast->objects = temp;
                } else {
                    _GSERR_s(parser->e, parser->current_token->line,errGSb02);
                }
                ast->objects[ast->objects_size-1] = res;
            }
        }
        parser_eat(parser, T_SEMI);
    }
    parser_eat(parser, T_RBR);
    return ast;
}
AST *parser_parse_obj_access(Parser *parser, Scope *scope) {
    //Set left to obj, right to the rest, then the whole thing back to obj iteratively
    AST *obj = parser_parse_mono(parser, scope);
    //detected list access
    if(parser->current_token->type == T_ARROW) {
        return parser_parse_list_arrow(obj, parser, scope);
    }
    if(parser->current_token->type != T_DOT) {
        return obj;
    }
    while(parser->current_token->type == T_DOT) {
        parser_eat(parser, T_DOT);
        char *name = parser->current_token->value;
        parser_eat(parser, T_ID);
        AST *memb = ast_init(AST_VAR);
        memb->line = parser->previous_token->line;
        memb->var_name = name;
        memb->scope = scope;
        if(parser->current_token->type == T_LPR) {
            memb = parser_parse_function_call(parser, scope);
        }

        AST *full = ast_init(AST_OBJACCESS);
        full->objaccess_left = obj;
        full->objaccess_right = memb;
        obj = full;
    }
    if(parser->current_token->type == T_EQL) {
        AST *obj_r = ast_init(AST_OBJ_REASSIGN);
        obj_r->obj_reassign_left = obj;
        obj_r->line = parser->current_token->line;
        parser_eat(parser, T_EQL);
        obj_r->obj_reassign_right = parser_parse_comp_expr(parser, scope);
        return obj_r;
    }
    return obj;
}
AST *parser_parse_field_or_method(Parser *parser, Scope *scope) {
    char *temp = parser->current_token->value;
    AST *res = NULL;
    if(strcmp(temp, "field") == 0) {
        res = parser_parse_variable_definition(parser, scope);
        return res;

    } else if (strcmp(temp, "method") == 0) {
        res = parser_parse_function_definition(parser, scope);
        return res;
    } else if (strcmp(temp, "obj")==0) {
        res = parser_parse_obj(parser, scope);
        return res;
    } else {
        _GSERR_s(parser->e, parser->current_token->line, errGS304);
    }
    return ast_init(AST_NOOP);
}
AST *parser_parse_import(Parser *parser, Scope *scope) {
    parser_eat(parser, T_ID);
    AST *imp = ast_init(AST_IMPORT);
    if(parser->current_token->type == T_PEXPR) {
        if(strcmp(parser->current_token->value, "math") == 0) {
            imp->import_val = "builtin_math.gs";
            imp->import_is_builtin = 1;
        } else {
            _GSERR_s(parser->e, parser->current_token->line, errGS108, parser->current_token->value);
            return ast_init(AST_NOOP);
        }
        parser_eat(parser, T_PEXPR);
    } else if (parser->current_token->type == T_STR) {
        imp->import_val = parser->current_token->value;
        parser_eat(parser, T_STR);
    } else {
        _GSERR_s(parser->e, parser->current_token->line, errGS107);
        return ast_init(AST_NOOP);
    }
    return imp;
}
AST *parser_parse_list_arrow(AST* initial, Parser *parser, Scope *scope) {
    AST *combined = initial;
    while(parser->current_token->type == T_ARROW) {
        parser_eat(parser, T_ARROW);
        AST* index = NULL;
        if(parser->current_token->type == T_NUM) {
            index = parser_parse_num(parser, scope);
        } else if(parser->current_token->type == T_ID) {
            char* name = parser->current_token->value;
            index = ast_init(AST_VAR);
            index->var_name = name;
            index->line = parser->current_token->line;
            index->scope = scope;
            parser_eat(parser, T_ID);
            if(parser->current_token->type == T_LPR) {
                index = parser_parse_function_call(parser, scope);
            }
        } else {
            _GSERR_s(parser->e, parser->current_token->line, errGS305);
            return ast_init(AST_NOOP);
        }

        AST *full = ast_init(AST_LIST_ARROW);
        full->list_arrow_left = combined;
        full->list_arrow_right = index;
        combined = full;
    }
    if(parser->current_token->type == T_EQL) {
        AST *list_r = ast_init(AST_LIST_REASSIGN);
        list_r->list_reassign_left = combined;
        list_r->line = parser->current_token->line;
        parser_eat(parser, T_EQL);
        list_r->list_reassign_right = parser_parse_comp_expr(parser, scope);
        return list_r;
    }
    return combined;
}