#include "core/lexer.h"
#include "core/parser.h"
#include "core/visitor.h"
#include "util/io.h"
#include "extra/exception.h"
#define _GS_GET_SRC get_file_data(ENTRYPOINT)
#define ENTRYPOINT "../gs-entry/entry.gs"

int main(int argc, char* argv[]) {
    Lexer *lexer = lexer_init( _GS_GET_SRC );
    ErrorStack *error_stack = lexer->e;
    Parser *parser = parser_init(lexer);
    AST *root = parser_parse(parser, parser->scope);
    Visitor *visitor = visitor_init(parser->e);

    visitor_visit(visitor, root);
    printf(color(BLU, "Program terminated with %d error(s)"), error_stack->errors_size);
    _print_err(error_stack);
    //printf(WHT);
    return 0;
}