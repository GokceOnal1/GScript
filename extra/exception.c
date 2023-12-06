#include "exception.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>



void _GSERR(unsigned line, char *description) {
    /*col(RED);
    printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\ngscript entry: (Line %d):\n", line);
    col(YEL);*/
    printf(color(RED, "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\ngscript \"entry.gs\": (Line %d):\n"), line);
    printf(color(YEL, "%s\n"), description);
    exit(1);

}
ErrorStack *errorstack_init() {
    ErrorStack *e = malloc(sizeof(ErrorStack));
    e->errors_size = 0;
    e->curr_file = "entry.gs";
    return e;
}
void _GSERR_s(ErrorStack *e, unsigned line, char *description, ...) {
    char desc_fmt[100];
    va_list args;
    va_start(args, description);
    vsprintf(desc_fmt, description, args);
    va_end(args);
    if (e->errors_size == 99) {
        printf("\n(Max Error Size exceeded)");
        _print_err(e);
        exit(1);
    } else {
        e->errors_size++;
        e->errors[e->errors_size-1] = calloc(150, sizeof(char));
        char d[100];
        sprintf(d, color(RED, "gscript: \"%s\" at line %d:\n")color(YEL,"%s\n"), e->curr_file, line, desc_fmt);
        strcpy(e->errors[e->errors_size-1], d);
    }
}
void _print_err(ErrorStack *e) {
    e->errors_size != 0 ? printf( color(RED,"\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n")) : printf("");
    for (int i = 0; i < e->errors_size; ++i) {
        printf(color(YEL, "%s"), e->errors[i]);
    }
    if(e->errors_size!=0)printf( color(RED,"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"));
}
void _terminate_gs(ErrorStack *e) {
    printf( color(RED, "Fatal Error: Program terminated early.\n"));
    printf( color(RED, "%d error(s)"), e->errors_size);
    _print_err(e);
    exit(1);
}
