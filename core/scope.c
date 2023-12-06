#include "scope.h"
#include <stdio.h>
#include <string.h>

Scope *scope_init(Scope *parent, ErrorStack *errorstack) {//printf("making scope");
    Scope *scope = calloc(1, sizeof(Scope));
    scope->parent = parent;
    scope->func_defs = (void *)0;
    scope->func_defs_size = 0;
    scope->var_defs = (void *)0;
    scope->var_defs_size = 0;
    scope->e = errorstack;
    return scope;
}
AST *scope_add_function_definition(Scope *scope, AST *fdef) {
    AST* existing = scope_get_function_definition(scope, fdef->func_def_name);
    if(existing != NULL) {
        _GSERR_s(scope->e, fdef->line, errGS505, fdef->func_def_name);
    }
    scope->func_defs_size++;
    if (scope->func_defs == (void *)0x0) {
        scope->func_defs = calloc(1, sizeof(AST *));
    } else {
        scope->func_defs =
                realloc(scope->func_defs, scope->func_defs_size * sizeof(AST*));
    }
    scope->func_defs[scope->func_defs_size - 1] = fdef;
    return fdef;
}

AST *scope_get_function_definition(Scope *scope, const char *fname) {
    while (scope != NULL) {
        for (int i = 0; i < scope->func_defs_size; i++) {
            AST *fdef = scope->func_defs[i];
            if (strcmp(fdef->func_def_name, fname) == 0) {
                return fdef;
            }
        }
        scope = scope->parent;
    }

    return NULL;
}

void scope_add_var_def(Scope *scope, AST *vdef) {
    AST *existing = scope_get_var_def(scope, vdef->var_def_var_name);

    if (existing != NULL) {
      //  printf("null!");
        _GSERR_s(scope->e, vdef->line, errGS302, vdef->var_def_var_name);
        return;
    }
    scope->var_defs_size++;
    if (scope->var_defs == NULL) {
        scope->var_defs = malloc( sizeof(AST *));
    } else {
        scope->var_defs =
                realloc(scope->var_defs, scope->var_defs_size * sizeof(AST *));
    }
    scope->var_defs[scope->var_defs_size - 1] = vdef;
  // printf("added var");
}

AST *scope_get_var_def(Scope *scope, const char *vname) {
    while (scope != NULL) {
        for (int i = 0; i < scope->var_defs_size; i++) {
            AST *var_def = scope->var_defs[i];
           // printf("\n%d: var in scope: %s", i, var_def->var_def_var_name);
            if (strcmp(var_def->var_def_var_name, vname) == 0) {
               //   printf("\nvar: %f ", var_def->var_def_value->num_value);
                return var_def->var_def_value;
            }
        }
        scope = scope->parent;
    }
    return NULL;
}

void scope_set_var(Scope *scope, AST *newdef) {
    while (scope != NULL) {
     //   printf("%d", scope->var_defs_size);
        for (int i = 0; i < scope->var_defs_size; i++) {
          //  printf(" j ");
            AST *var_def = scope->var_defs[i];
            //printf("%s <-> %s\n", var_def->var_def_var_name, newdef->var_def_var_name);
            if (strcmp(var_def->var_def_var_name, newdef->var_def_var_name) == 0) {

                scope->var_defs[i]->var_def_value = newdef->var_def_value;
                // printf("\nThe extern def: %f", newdef->var_def_value->num_value);
                // printf("\nThe scoped def: %f",
                // scope->var_defs[i]->var_def_value->num_value);
                return;
            }
        }
        scope = scope->parent;
    }
    _GSERR_s(scope->e, newdef->line, errGS303,
             newdef->var_def_var_name);
}
void scope_clear_defs(Scope* scope) {
    scope->func_defs = NULL;
    scope->func_defs_size = 0;
    scope->var_defs = NULL;
    scope->var_defs_size = 0;
}