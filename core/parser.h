#ifndef GSCRIPT2_PARSER_H
#define GSCRIPT2_PARSER_H
#include "../struct/AST.h"
#include "lexer.h"
#include "scope.h"
#include "../extra/exception.h"
#include <stdio.h>
#include <string.h>

typedef struct parser_s {
    Lexer *lexer;
    Token *current_token;
    Token *previous_token;
    Scope *scope;
    ErrorStack *e;

} Parser;

Parser *parser_init(Lexer *lexer);
void parser_eat(Parser *parser, int token_type);
AST *parser_parse(Parser *parser, Scope *scope);
AST *parser_parse_statement(Parser *parser, Scope *scope);
AST *parser_parse_statements(Parser *parser, Scope *scope);
AST *parser_parse_expr(Parser *parser, Scope *scope);
AST *parser_parse_comp_expr(Parser *parser, Scope *scope);
AST *parser_parse_comp_term(Parser *parser, Scope *scope);
AST *parser_parse_factor(Parser *parser, Scope *scope);
AST *parser_parse_mono(Parser *parser, Scope *scope);
AST *parser_parse_term(Parser *parser, Scope *scope);
AST *parser_parse_function_call(Parser *parser, Scope *scope);
AST *parser_parse_variable(Parser *parser, Scope *scope);
AST *parser_parse_string(Parser *parser, Scope *scope);
AST *parser_parse_id(Parser *parser, Scope *scope);
AST *parser_parse_num(Parser *parser, Scope *scope);
AST *parser_parse_var_reassign(Parser *parser, Scope *scope);
AST *parser_parse_variable_definition(Parser *parser, Scope *scope);
AST *parser_parse_function_definition(Parser *parser, Scope *scope);
AST *parser_parse_if_statement(Parser *parser, Scope *scope);
AST *parser_parse_repeat_statement(Parser *parser, Scope *scope);
AST *parser_parse_leave_statement(Parser *parser, Scope *scope);
AST *parser_parse_return_statement(Parser *parser, Scope *scope);
AST *parser_parse_while_statement(Parser *parser, Scope *scope);
AST *parser_parse_function_param(Parser *parser, Scope *scope);
AST *parser_parse_list_body(Parser *parser, Scope *scope);
AST *parser_parse_list_sub_index(Parser *parser, Scope *scope);
AST *parser_parse_obj(Parser *parser, Scope *scope);
AST *parser_parse_obj_access(Parser *parser, Scope *scope);
AST *parser_parse_field_or_method(Parser *parser, Scope *scope);

#endif //GSCRIPT2_PARSER_H
