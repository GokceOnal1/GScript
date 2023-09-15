#include "token.h"
#include <stdlib.h>
/*Constructs a Token with type TYPE and value VALUE*/
Token* token_init(int type, char* value, unsigned int line) {
    Token* token = calloc(1, sizeof(Token));
    token->type = type;
    token->value = value;
    token->line = line;
    return token;
}