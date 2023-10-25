#ifndef GSCRIPT2_DEFS_H
#define GSCRIPT2_DEFS_H

#define MAXLINE 10000
#define stringify( s ) # s
#define cat( x, y ) x ## y
#define MATH_PI 3.14159265358979323846
#define MATH_E 2.7

/*
 * obj a = {
 * field b = 5;
 * method c(param p) { write(p); };
 * method d() { write(b); };
 *
 * };
 *
 * import a from "stddefs.gs"
 * import all from "stddefs.gs"
 *
 * write(a.b);
 */


#endif //GSCRIPT2_DEFS_H
