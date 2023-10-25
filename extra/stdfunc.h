#ifndef GSCRIPT2_STDFUNC_H
#define GSCRIPT2_STDFUNC_H

#include "exception.h"
#include "../struct/AST.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "../core/visitor.h"
AST *std_func_write(Visitor *visitor, AST **argv, int argc, unsigned line);
AST *std_func_read(Visitor *visitor, AST **argv, int argc, unsigned line);
AST *std_func_toupper(Visitor *visitor, AST **argv, int argc, unsigned line);
AST *std_func_tolower(Visitor *visitor, AST **argv, int argc, unsigned line);
AST *std_func_randnum(Visitor *visitor, AST **argv, int argc, unsigned line);
AST *std_func_num(Visitor *visitor, AST **argv, int argc, unsigned line);
AST *std_func_len(Visitor *visitor, AST **argv, int argc, unsigned line);
AST *std_func_strindx(Visitor *visitor, AST **argv, int argc, unsigned line);
AST *std_func_listindx(Visitor *visitor, AST **argv, int argc, unsigned line);
AST *std_func_listadd(Visitor *visitor, AST **argv, int argc, unsigned line);

#endif //GSCRIPT2_STDFUNC_H
