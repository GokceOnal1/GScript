#ifndef LEXER_H
#define LEXER_H
#include "token.h"
typedef struct lexer_s {
	char c;
	unsigned int i;
  unsigned int line;
	char* contents;
	
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
Token* lexer_advance_with_token(Lexer* lexer, Token* token); //Returns token and also advances c

#endif
