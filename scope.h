#ifndef SCOPE_H
#define SCOPE_H
#include "AST.h"
typedef struct scope_s {
	AST** func_defs;
	size_t func_defs_size;
} Scope;

Scope* scope_init();
AST* scope_add_function_definition(Scope* scope, AST* fdef);
AST* scope_get_function_definition(Scope* scope, const char *fname);

#endif