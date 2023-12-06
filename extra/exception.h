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

#define errGSb01 "GS.b.01 - Failed reading from entry input stream"
#define errGSb02 "GS.b.02 - Internal null pointer"
#define errGS101 "GS.101 - Expected token '%s' but received token '%s' (type '%s')"
#define errGS102 "GS.102 - Unsupported character %s"
#define errGS103 "GS.103 - Expected '=' after '!'"
#define errGS105 "GS.105 - Unrelated 'elsif' caught"
#define errGS106 "GS.106 - Unrelated 'else' caught"
#define errGS107 "GS.107 - Expected a string or macro expression after 'import'"
#define errGS108 "GS.108 - Undefined macro expression '%s'"
#define errGS201 "GS.201 - Uncaught statement of type %d"
#define errGS202 "GS.202 - Expected condition expression"
#define errGS301 "GS.301 - Undefined identifier %s"
#define errGS302 "GS.302 - Variable '%s' already exists in the current scope"
#define errGS303 "GS.303 - Variable '%s' does not exist in the current scope"
#define errGS304 "GS.304 - Expected 'field', 'method', or 'obj' for object definition"
#define errGS401 "GS.401 - Object '%s' already exists in the current scope"
#define errGS402 "GS.402 - Object '%s' does not exist in the current scope"
#define errGS403 "GS.403 - Member '%s' of object '%s' does not exist"
#define errGS404 "GS.404 - Member access attempted on non-object"
#define errGS501 "GS.501 - No arguments provided where needed"
#define errGS502 "GS.502 - Function '%s' does not take %d arguments"
#define errGS503 "GS.503 - Function '%s' does not exist in the current scope"
#define errGS504 "GS.504 - Illegal argument to function '%s' (type '%s')"
#define errGS505 "GS.505 - Function '%s' already exists in the current scope"
#define errGS506 "GS.506 - Expected keyword 'param' for function parameters"
#define errGS601 "GS.601 - Dividing by zero is illegal"
#define errGS602 "GS.602 - Invalid syntax for a number"
#define errGS603 "GS.603 - Operator '+' only supports num+num or str+str"
#define errGS604 "GS.604 - Illegal operand"

typedef struct error_s {
    unsigned errors_size;
    char *errors[100];
    char *curr_file;
} ErrorStack;


void _GSERR(unsigned line, char *description) __attribute__((deprecated));

ErrorStack *errorstack_init();
void _GSERR_s(ErrorStack *e, unsigned line, char *description, ...);
void _print_err(ErrorStack *e);
void _terminate_gs(ErrorStack *e);
#endif //GSCRIPT2_EXCEPTION_H
