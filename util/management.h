//
// Created by obona on 3/18/2024.
//

#ifndef GSCRIPT2_MANAGEMENT_H
#define GSCRIPT2_MANAGEMENT_H

#include "../core/lexer.h"
#include "../core/parser.h"

void MEM_free_ast(AST*);
void MEM_copy_ast(AST*, AST*);
void MEM_copy_ast_arr(AST**, AST**, size_t);
void MEM_free_ast_arr(AST**, size_t);
void visualize_ast(AST*, unsigned);
void visualize_ast_arr(AST**, size_t, unsigned);


#endif //GSCRIPT2_MANAGEMENT_H
