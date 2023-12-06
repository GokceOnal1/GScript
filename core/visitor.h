#ifndef GSCRIPT2_VISITOR_H
#define GSCRIPT2_VISITOR_H
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include "scope.h"
#include "lexer.h"
#include "visitor.h"
#include "../extra/exception.h"
#include "../struct/token.h"
#include "../util/debug.h"
#include "../util/io.h"


typedef struct visitor_s {
    AST **var_defs;
    size_t var_defs_size;
    Scope *current_scope;
    ErrorStack *e;
} Visitor;

Visitor *visitor_init(ErrorStack *errorstack);
AST *visitor_visit(Visitor *visitor, AST *node);
AST *visitor_visit_var_def(Visitor *visitor, AST *node);
AST *visitor_visit_var_reassign(Visitor *visitor, AST *node);
AST *visitor_visit_func_def(Visitor *visitor, AST *node);
AST *visitor_visit_var(Visitor *visitor, AST *node);
AST *visitor_visit_func_call(Visitor *visitor, AST *node, Scope *calledfrom);
AST *visitor_visit_str(Visitor *visitor, AST *node);
AST *visitor_visit_num(Visitor *visitor, AST *node);
AST *visitor_visit_binop(Visitor *visitor, AST *node);
AST *visitor_visit_bool(Visitor *visitor, AST *node);
AST *visitor_visit_if_statement(Visitor *visitor, AST *node);
AST *visitor_visit_repeat_statement(Visitor *visitor, AST *node);
AST *visitor_visit_while_statement(Visitor *visitor, AST *node);
AST *visitor_visit_unop(Visitor *visitor, AST *node);
AST* visitor_visit_listindx(Visitor *visitor, AST *node);
AST *visitor_visit_compound(Visitor *visitor, AST *node, unsigned change_scope);
AST *visitor_visit_obj_def(Visitor *visitor, AST *node);
AST *visitor_visit_objaccess(Visitor *visitor, AST *node);
AST *visitor_convert_to_str(Visitor *visitor, AST *node);
AST *visitor_visit_obj_reassign(Visitor *visitor, AST *node);
char *visitor_scope_to_objacc(Visitor *visitor, AST *node);
AST *visitor_visit_import(Visitor *visitor, AST *node);

#endif //GSCRIPT2_VISITOR_H
