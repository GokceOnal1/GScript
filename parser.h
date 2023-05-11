#ifndef PARSER_H
#define PARSER_H
#include "AST.h"
#include "lexer.h"
#include "scope.h"

typedef struct parser_s {
  Lexer *lexer;
  Token *current_token;
  Token *previous_token;
  Scope *scope;
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
AST *parser_parse_term(Parser *parser, Scope *scope);
AST *parser_parse_function_call(Parser *parser, Scope *scope);
AST *parser_parse_variable(Parser *parser, Scope *scope);
AST *parser_parse_string(Parser *parser, Scope *scope);
AST *parser_parse_id(Parser *parser, Scope *scope);
AST *parser_parse_num(Parser *parser, Scope *scope);
AST *parser_parse_variable_definition(Parser *parser, Scope *scope);
AST *parser_parse_function_definition(Parser *parser, Scope *scope);

#endif