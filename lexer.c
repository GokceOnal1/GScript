#include "lexer.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "exception.h"

/*Constructs a new Lexer with CONTENTS as the data*/
Lexer *lexer_init(char *contents) {
  Lexer *lexer = calloc(1, sizeof(Lexer));
  lexer->contents = contents;
  lexer->i = 0;
  lexer->line = 1;
  lexer->c = contents[lexer->i];

  return lexer;
}
/*Increments C to the next char in CONTENTS*/
void lexer_advance(Lexer *lexer) {
  if (lexer->c != '\0' && lexer->i < strlen(lexer->contents)) {
    lexer->i++;
    lexer->c = lexer->contents[lexer->i];
    if(lexer->c == '\n') { 
      lexer->line++;
      //printf("%d", lexer->line);
    }
  }
}
/*Advances C until the char in CONTENTS is not a whitespace or a newline*/
void lexer_skip_space(Lexer *lexer) {
  while (lexer->c == ' ' || lexer->c == 10 || lexer->c == '\t') {
    lexer_advance(lexer);
  }
}
/*Advanced C until the char in CONTENTS is not inside a comment ($)*/
void lexer_skip_comments(Lexer *lexer) {
	lexer_advance(lexer);
	while(lexer->c != '$' && lexer->c != '\0') lexer_advance(lexer);
	lexer_advance(lexer);
  lexer_skip_space(lexer);
}
/*Returns next token from CONTENTS, or (void*)0 if there are none left*/
Token *lexer_get_next_token(Lexer *lexer) {
  while (lexer->c != '\0' && lexer->i < strlen(lexer->contents)) {

		
    if (lexer->c == ' ' || lexer->c == '\n' || lexer->c == '\t')
      lexer_skip_space(lexer);
    if(lexer->c == '$') {
     // printf("hey");
			lexer_skip_comments(lexer);
		}
    if(lexer->c == 0) return token_init(T_EOF, "\0", lexer->line);
    if (lexer->c == '"') {
      return lexer_collect_str(lexer);
    }
    if (isalpha(lexer->c) || lexer->c == '_') {
      return lexer_collect_id(lexer);
    }
    if(isdigit(lexer->c)) {
      return lexer_collect_num(lexer);
    }
    if(lexer->c == '.') {
      _GSERR(lexer->line, "GS602 - Invalid syntax for a number");
    }

    switch (lexer->c) {
    case '=':
      return /*lexer_advance_with_token(
          lexer, token_init(T_EQL, lexer_get_current_char_as_str(lexer), lexer->line));*/
          lexer_collect_eq(lexer);
      break;
      case '<':
        return lexer_collent_lt(lexer); break;
      case '>':
        return lexer_collect_gt(lexer); break;
      case '!':
        return lexer_collect_neq(lexer); break;
      case '&':
        return lexer_advance_with_token(
          lexer, token_init(T_AND, lexer_get_current_char_as_str(lexer), lexer->line));
      break;
      case '|':
        return lexer_advance_with_token(
          lexer, token_init(T_OR, lexer_get_current_char_as_str(lexer), lexer->line));
      break;
    case ';':
      return lexer_advance_with_token(
          lexer, token_init(T_SEMI, lexer_get_current_char_as_str(lexer), lexer->line));
      break;
    case '(':
      return lexer_advance_with_token(
          lexer, token_init(T_LPR, lexer_get_current_char_as_str(lexer), lexer->line));
      break;
    case ')':
      return lexer_advance_with_token(
          lexer, token_init(T_RPR, lexer_get_current_char_as_str(lexer), lexer->line));
		case ',':
      return lexer_advance_with_token(
          lexer, token_init(T_COMMA, lexer_get_current_char_as_str(lexer), lexer->line));
		case '{':
      return lexer_advance_with_token(
          lexer, token_init(T_LBR, lexer_get_current_char_as_str(lexer), lexer->line));
      case '+': 
        return lexer_advance_with_token(lexer, token_init(T_PLUS, lexer_get_current_char_as_str(lexer), lexer->line));
      case '-':
      return lexer_advance_with_token(lexer, token_init(T_MIN, lexer_get_current_char_as_str(lexer), lexer->line));
      case '*':
      return lexer_advance_with_token(lexer, token_init(T_MUL, lexer_get_current_char_as_str(lexer), lexer->line));
      case '/':
      return lexer_advance_with_token(lexer, token_init(T_DIV, lexer_get_current_char_as_str(lexer), lexer->line));
		case '}':
      return lexer_advance_with_token(
          lexer, token_init(T_RBR, lexer_get_current_char_as_str(lexer), lexer->line));
      break;
     
		default:
      {
        //printf("%d", lexer->c);
        char d[100];
        sprintf(d, "GS102 - Unsupported character '%c'", lexer->c);
        _GSERR(lexer->line, d);
        exit(1);
        lexer_advance(lexer);
        break;
      }
    }
  }
  return token_init(T_EOF, "\0", lexer->line);
}
/*Returns next string value in CONTENTS*/
Token *lexer_collect_str(Lexer *lexer) {
  lexer_advance(lexer);
  char *value = calloc(2, sizeof(char));
  value[0] = '\0';
  while (lexer->c != '"') {
    char *s = lexer_get_current_char_as_str(lexer);
    value = realloc(value, (strlen(value) + strlen(s) + 1) * sizeof(char));
    strcat(value, s);
    lexer_advance(lexer);
  }
  lexer_advance(lexer);
  return token_init(T_STR, value, lexer->line);
}
/*Returns next idetifier value in CONTENTS*/
Token *lexer_collect_id(Lexer *lexer) {
  char *value = calloc(1, sizeof(char));
  value[0] = '\0';
  while (isalnum(lexer->c) || lexer->c=='_') {
    char *s = lexer_get_current_char_as_str(lexer);
    value = realloc(value, (strlen(value) + strlen(s) + 1) * sizeof(char));
    strcat(value, s);
    lexer_advance(lexer);
  }
  return token_init(T_ID, value, lexer->line);
}
/*Returns next number in CONTENTS*/
Token *lexer_collect_num(Lexer *lexer) {
  char *value = calloc(1, sizeof(char));
  value[0] = '\0';
  int dc = 0;
  while (isdigit(lexer->c) || lexer->c == '.') {
    if(lexer->c == '.') dc++;
    if(dc > 1) _GSERR(lexer->line, "GS602 - Invalid syntax for a number");
    char *s = lexer_get_current_char_as_str(lexer);
    value = realloc(value, (strlen(value) + strlen(s) + 1) * sizeof(char));
    strcat(value, s);
    lexer_advance(lexer);
  }
  return token_init(T_NUM, value, lexer->line);
}
/*Returns C as a string to be used as  VALUE for a token*/
char *lexer_get_current_char_as_str(Lexer *lexer) {
  char *s = calloc(2, sizeof(char));
  s[0] = lexer->c;
  s[1] = '\0';
  return s;
}
Token *lexer_collect_eq(Lexer *lexer){
  int type = T_EQL;
  char *value = lexer_get_current_char_as_str(lexer);
  lexer_advance(lexer);
  if(lexer->c == '=') {
    lexer_advance(lexer);
    type = T_EE;
    value = realloc(value, 3 * sizeof(char));
    value[0] = '=';
    value[1] = '=';
    value[2] = '\0';
  }
  return token_init(type, value, lexer->line);
}
Token *lexer_collent_lt(Lexer *lexer){
  int type = T_LT;
  char *value = lexer_get_current_char_as_str(lexer);
  lexer_advance(lexer);
  if(lexer->c == '=') {
    lexer_advance(lexer);
    type = T_LTE;
    value = realloc(value, 3 * sizeof(char));
    value[0] = '<';
    value[1] = '=';
    value[2] = '\0';
  }
  return token_init(type, value, lexer->line);
}
Token *lexer_collect_gt(Lexer *lexer){
  int type = T_GT;
  char *value = lexer_get_current_char_as_str(lexer);
  lexer_advance(lexer);
  if(lexer->c == '=') {
    lexer_advance(lexer);
    type = T_GTE;
    value = realloc(value, 3 * sizeof(char));
    value[0] = '>';
    value[1] = '=';
    value[2] = '\0';
  }
  return token_init(type, value, lexer->line);
}
Token *lexer_collect_neq(Lexer *lexer){
  char *value = lexer_get_current_char_as_str(lexer);
  lexer_advance(lexer);
  if(lexer->c == '=') {
    lexer_advance(lexer);
    value = realloc(value, 3 * sizeof(char));
    value[0] = '!';
    value[1] = '=';
    value[2] = '\0';
    return token_init(T_NEQ, value, lexer->line);
  } 
  _GSERR(lexer->line, "GS104 - Expected '=' after '!'");
  return NULL;
}
/*Returns given TOKEN and also advances C*/
Token *lexer_advance_with_token(Lexer *lexer, Token *token) {
  lexer_advance(lexer);
  return token;
}
