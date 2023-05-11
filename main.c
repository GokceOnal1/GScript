#include "lexer.h"
#include "parser.h"
#include "visitor.h"
#include <stdio.h>
#include "io.h"

int main(int argc, char* argv[]) {
  Lexer *lexer = lexer_init( get_file_data("entry.gs") );
  Parser *parser = parser_init(lexer);
	AST *root = parser_parse(parser, parser->scope);
	Visitor *visitor = visitor_init();

	visitor_visit(visitor, root);
	
  return 0;
}