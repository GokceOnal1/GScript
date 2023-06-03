#include "parser.h"
#include "exception.h"
#include "scope.h"
#include <stdio.h>
#include <string.h>

/* This works by basically constructing a tree (called Abstract Syntax Tree or
 * AST) of values and operators for the Interpreter to interpret in the future.
 * The parser makes nodes in the tree by reading (or 'parsing') tokens that have
 * been lexed by the Lexer. */

/*Constructs a Parser based on the LEXER*/
Parser *parser_init(Lexer *lexer) {
  Parser *parser = calloc(1, sizeof(Parser));
  parser->lexer = lexer;
  parser->current_token = lexer_get_next_token(lexer);
  parser->previous_token = parser->current_token;
  parser->scope = scope_init(NULL);
  return parser;
}
/*Tells the PARSER what TOKEN_TYPE to expect from the lexer. Kind of like
 * lexer_advance()*/
void parser_eat(Parser *parser, int token_type) {
  if (parser->current_token->type == token_type) {
    parser->previous_token = parser->current_token;
    parser->current_token = lexer_get_next_token(parser->lexer);
  } else {
    char d[100];
    sprintf(d, "GS101 - Expected %d. Unexpected token '%s' (of type %d)",
            token_type, parser->current_token->value,
            parser->current_token->type);
    _GSERR(parser->current_token->line, d);
    exit(1);
  }
}
/*Parses the next statement or statements from the lexer and returns AST Node
 * with value*/
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

    AST *ast_statement = parser_parse_comp_expr(parser, scope);
    if (ast_statement) {
      compound->compound_size++;
      compound->compound_value = realloc(
          compound->compound_value, compound->compound_size * sizeof(AST *));
      compound->compound_value[compound->compound_size - 1] = ast_statement;
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
    AST *ast_expr = parser_parse_comp_expr(parser, scope);
    function_call->func_call_args_size++;
    function_call->func_call_args =
        realloc(function_call->func_call_args,
                function_call->func_call_args_size * sizeof(AST *));
    function_call->func_call_args[function_call->func_call_args_size - 1] =
        ast_expr;
  }
  parser_eat(parser, T_RPR);
  function_call->scope = scope;
  return function_call;
}
/*Parses a variable reference from the lexer and returns AST Node with relevant
 * information*/
AST *parser_parse_variable(Parser *parser, Scope *scope) {
  char *token_value = parser->current_token->value;
  parser_eat(parser, T_ID); // Either var name, or func call name
  if (parser->current_token->type == T_LPR) {
    return parser_parse_function_call(parser, scope);
  } else if (parser->current_token->type == T_EQL) {
    return parser_parse_var_reassign(parser, scope);
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
    _GSERR(parser->current_token->line, "GS105 - Unrelated 'elsif' caught");
    return NULL;
  } else if (strcmp(parser->current_token->value, "else") == 0) {
    _GSERR(parser->current_token->line, "GS106 - Unrelated 'else' caught");
    return NULL;
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
  double value = strtod(parser->current_token->value, NULL);
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
  parser_eat(parser, T_ID);
  char *func_name = parser->current_token->value;
  ast->line = parser->current_token->line;
  ast->func_def_name = calloc(strlen(func_name) + 1, sizeof(char));
  strcpy(ast->func_def_name, func_name);
  parser_eat(parser, T_ID);
  parser_eat(parser, T_LPR);
  // Parses parameters of a function definition
  if (parser->current_token->type != T_RPR) {
    ast->func_def_args = calloc(1, sizeof(AST *));
    AST *arg = parser_parse_function_param(parser, scope);
    ast->func_def_args_size++;
    ast->func_def_args[ast->func_def_args_size - 1] = arg;
    scope_add_var_def(scope, arg);
  }

  while (parser->current_token->type == T_COMMA) {
    parser_eat(parser, T_COMMA);
    ast->func_def_args_size++;
    ast->func_def_args =
        realloc(ast->func_def_args, ast->func_def_args_size * sizeof(AST *));
    AST *arg = parser_parse_function_param(parser, scope);
    ast->func_def_args[ast->func_def_args_size - 1] = arg;
    scope_add_var_def(scope, arg);
  }

  parser_eat(parser, T_RPR);
  parser_eat(parser, T_LBR);
  ast->func_def_body = parser_parse_statements(parser, scope);
  parser_eat(parser, T_RBR);
  ast->scope = scope;
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
  return ast;
}
AST *parser_parse_leave_statement(Parser *parser, Scope *scope) {
  AST *ast = ast_init(AST_OUT);
  parser_eat(parser, T_ID);
  return ast;
}
AST *parser_parse_return_statement(Parser *parser, Scope *scope) {
  AST *ast = ast_init(AST_RETURN);
  parser_eat(parser, T_ID);
  ast->should_return = 1;
  ast->return_value = parser_parse_comp_expr(parser, scope);
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
  return ast;
}
AST *parser_parse_function_param(Parser *parser, Scope *scope) {
  AST *ast = ast_init(AST_VAR_DEF);
  if (strcmp(parser->current_token->value, "param") != 0) {
    _GSERR(parser->current_token->line, "Expected keyword 'param' for declaring function parameters");
  }
  parser_eat(parser, T_ID);
  ast->var_def_var_name = parser->current_token->value;
  parser_eat(parser, T_ID);
  ast->var_def_value = ast_init(AST_NUM);
  return ast;
}
/*static unsigned is_keyword(const char *arg) {
  char *keywords[] = {"assign", "funct", "if"};
  size_t k_size = 3;
  for (int i = 0; i < k_size; ++i) {
    if (strcmp(arg, keywords[i]) == 0) {
      return 1;
    }
  }
  return 0;
}*/