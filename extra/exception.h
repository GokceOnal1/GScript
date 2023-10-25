#ifndef GSCRIPT2_EXCEPTION_H
#define GSCRIPT2_EXCEPTION_H
#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define YEL "\x1B[33m"
#define BLU "\x1B[34m"
#define MAG "\x1B[35m"
#define CYN "\x1B[36m"
#define WHT "\x1B[37m"
#define RESET "\x1B[0m"

#define clruse 1
#define nulltext "GS.b.02 - Internal null pointer"
#if clruse
#define color(col, str) col str "\x1b[0m"
#else
#define color(col, str) str
#endif

typedef struct error_s {
    unsigned errors_size;
    char *errors[100];
} ErrorStack;


void _GSERR(unsigned line, char *description) __attribute__((deprecated));

ErrorStack *errorstack_init();
void _GSERR_s(ErrorStack *e, unsigned line, char *description, ...);
void _print_err(ErrorStack *e);
void _terminate_gs(ErrorStack *e);
#endif //GSCRIPT2_EXCEPTION_H
