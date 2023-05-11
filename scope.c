#include "scope.h"
#include <string.h>

Scope* scope_init() {
	Scope* scope = calloc(1, sizeof(Scope));
	scope->func_defs = (void*)0;
	scope->func_defs_size = 0;
	return scope;
}
AST* scope_add_function_definition(Scope* scope, AST* fdef) {
	scope->func_defs_size++;
	if(scope->func_defs == (void*)0) {
		scope->func_defs = calloc(1, sizeof(AST*));
	} else {
		scope->func_defs = realloc(scope->func_defs, scope->func_defs_size * sizeof(Scope**));
	}
	scope->func_defs[scope->func_defs_size-1] = fdef;
	return fdef;
}

AST* scope_get_function_definition(Scope* scope, const char *fname) {
	for(int i = 0; i < scope->func_defs_size; i++) {
		AST* fdef= scope->func_defs[i];
		if(strcmp(fdef->func_def_name, fname)==0) {
			return fdef;
		}
	}
	return (void*)0;
}