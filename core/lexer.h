#ifndef GSCRIPT2_LEXER_H
#define GSCRIPT2_LEXER_H
#include "../extra/exception.h"
#include "../struct/token.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct lexer_s {
    char c;
    unsigned int i;
    unsigned int line;
    char* contents;
    ErrorStack* e;

} Lexer;

Lexer* lexer_init(char* contents); //Lexer constructor
void lexer_advance(Lexer* lexer); //Increments c to next char in contents
void lexer_skip_space(Lexer* lexer); //Skips whitespaces in contents
void lexer_skip_comments(Lexer* lexer); //Skips comments in contents
Token* lexer_get_next_token(Lexer* lexer); //Gets next token from contents
Token* lexer_collect_str(Lexer* lexer); //Gets string from contents
Token* lexer_collect_id(Lexer* lexer); //Gets IDENTIFIER from contents
Token *lexer_collect_num(Lexer *lexer);
char* lexer_get_current_char_as_str(Lexer* lexer); //Return c as string
Token *lexer_collect_eq(Lexer *lexer);
Token *lexer_collent_lt(Lexer *lexer);
Token *lexer_collect_gt(Lexer *lexer);
Token *lexer_collect_neq(Lexer *lexer);
Token *lexer_collect_pexpr(Lexer *lexer);
Token *lexer_collect_arrow(Lexer *lexer);
Token* lexer_advance_with_token(Lexer* lexer, Token* token); //Returns token and also advances c

#endif //GSCRIPT2_LEXER_H
