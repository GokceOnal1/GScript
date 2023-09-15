#ifndef GSCRIPT2_TOKEN_H
#define GSCRIPT2_TOKEN_H
typedef struct token_s {
    enum { T_ID, T_EQL, T_STR, T_SEMI, T_LPR, T_RPR, T_LBR, T_RBR, T_CMT, T_COMMA, T_EOF, T_NUM, T_PLUS, T_MIN, T_MUL, T_DIV, T_LT, T_GT, T_EE, T_LTE, T_GTE, T_NEQ, T_AND, T_OR, T_MOD, T_NOT, T_LSQB, T_RSQB, T_COLON, T_TILDE } type;
    char* value;
    unsigned int line;
} Token;

Token* token_init(int type, char* value, unsigned int line);
#endif //GSCRIPT2_TOKEN_H
