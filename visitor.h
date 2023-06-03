#ifndef VISITOR_H
#define VISITOR_H
#include "AST.h"
#include "scope.h"

typedef struct visitor_s {
  AST **var_defs;
  size_t var_defs_size;
  Scope *current_scope;
} Visitor;

Visitor *visitor_init();
AST *visitor_visit(Visitor *visitor, AST *node);
AST *visitor_visit_var_def(Visitor *visitor, AST *node);
AST *visitor_visit_var_reassign(Visitor *visitor, AST *node);
AST *visitor_visit_func_def(Visitor *visitor, AST *node);
AST *visitor_visit_var(Visitor *visitor, AST *node);
AST *visitor_visit_func_call(Visitor *visitor, AST *node);
AST *visitor_visit_str(Visitor *visitor, AST *node);
AST *visitor_visit_num(Visitor *visitor, AST *node);
AST *visitor_visit_binop(Visitor *visitor, AST *node);
AST *visitor_visit_bool(Visitor *visitor, AST *node);
AST *visitor_visit_if_statement(Visitor *visitor, AST *node);
AST *visitor_visit_repeat_statement(Visitor *visitor, AST *node);
AST *visitor_visit_while_statement(Visitor *visitor, AST *node);
AST *visitor_visit_unop(Visitor *visitor, AST *node);
AST *visitor_visit_compound(Visitor *visitor, AST *node);

#endif