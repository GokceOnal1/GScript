cmake_minimum_required(VERSION 3.26)
project(GScript2 C)

set(CMAKE_C_STANDARD 23)

add_executable(GScript2 main.c
        core/visitor.c
        core/visitor.h
        util/debug.h
        util/debug.c
        util/io.c
        util/io.h
        core/lexer.c
        core/lexer.h
        core/parser.c
        core/parser.h
        struct/AST.c
        struct/AST.h
        struct/token.c
        struct/token.h
        extra/exception.c
        extra/exception.h
        extra/stdfunc.c
        extra/stdfunc.h
        core/scope.c
        core/scope.h
        util/test.c
        util/test.h
        util/defs.h
)