#include "core/lexer.h"
#include "core/parser.h"
#include "core/visitor.h"
#include "util/io.h"
#include "extra/exception.h"
#include "util/management.h"
#define _GS_GET_SRC get_file_data(ENTRYPOINT)
#define ENTRYPOINT "../gs-entry/entry.gs"

int main(int argc, char* argv[]) {
    Lexer *lexer = lexer_init( _GS_GET_SRC );
    ErrorStack *error_stack = lexer->e;
    Parser *parser = parser_init(lexer);
    AST *root = parser_parse(parser, parser->scope);
    Visitor *visitor = visitor_init(parser->e);
    //importing specialized objects
    AST* __imp_list = ast_init(AST_IMPORT);
    __imp_list->import_is_builtin = 1;
    __imp_list->import_val = "builtin_list.gs";
    visitor_visit_import(visitor, __imp_list);

    visitor_visit(visitor, root);
//    AST* vdef = ast_init(AST_VAR_DEF);
//    vdef->var_def_var_name = "a";
//    vdef->var_def_value = ast_init(AST_NUM);
//    scope_set_var(visitor->current_scope, vdef);
//    AST* vdef2 = ast_init(AST_VAR_DEF);
//    vdef2->var_def_var_name = "b";
//    vdef2->var_def_value = scope_get_var_def(visitor->current_scope, "a");
//    visitor_visit_var_def(visitor, vdef2);
    printf(color(BLU, "Program terminated with %d error(s)"), error_stack->errors_size);
    _print_err(error_stack);
    //printf(WHT);
    return 0;
}