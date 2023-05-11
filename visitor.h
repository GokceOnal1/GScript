#ifndef VISITOR_H
#define VISITOR_H
#include "AST.h"

typedef struct visitor_s {
	AST** var_defs;
	size_t var_defs_size;
} Visitor;

Visitor* visitor_init();
AST* visitor_visit(Visitor* visitor, AST* node);
AST* visitor_visit_var_def(Visitor* visitor, AST* node);
AST* visitor_visit_func_def(Visitor* visitor, AST* node);
AST* visitor_visit_var(Visitor* visitor, AST* node);
AST* visitor_visit_func_call(Visitor* visitor, AST* node);
AST* visitor_visit_str(Visitor* visitor, AST* node);
AST* visitor_visit_num(Visitor* visitor, AST* node);
AST* visitor_visit_binop(Visitor* visitor, AST* node);
AST* visitor_visit_bool(Visitor* visitor, AST* node);
AST* visitor_visit_compound(Visitor* visitor, AST* node);

#endif