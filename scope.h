#ifndef SCOPE_H
#define SCOPE_H
#include "AST.h"
typedef struct scope_s {
  struct scope_s* parent;
	AST** func_defs;
	size_t func_defs_size;
  AST** var_defs;
  size_t var_defs_size;
} Scope;

Scope* scope_init(Scope* parent);
AST* scope_add_function_definition(Scope* scope, AST* fdef);
AST* scope_get_function_definition(Scope* scope, const char *fname);
void scope_add_var_def(Scope* scope, AST* vdef);
AST* scope_get_var_def(Scope* scope, const char *vname);
void scope_set_var(Scope* scope, AST* newdef);
void scope_clear_defs(Scope* scope);

#endif