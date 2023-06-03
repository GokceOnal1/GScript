#include "lexer.h"
#include "parser.h"
#include "visitor.h"
#include <stdio.h>
#include "io.h"
#define _GS_GET_SRC get_file_data(ENTRYPOINT)
#define ENTRYPOINT "entry.gs"

int main(int argc, char* argv[]) {
  Lexer *lexer = lexer_init( _GS_GET_SRC );
  Parser *parser = parser_init(lexer);
	AST *root = parser_parse(parser, parser->scope);
	Visitor *visitor = visitor_init();
  
	visitor_visit(visitor, root);
  return 0;
}