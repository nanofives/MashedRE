/* Fixture library for the binder's lib kind (re/tools/bind/test_porttop_bind_lib.py, BIND-LIB-ROUTE). Not game code. */
#ifndef LIBFIX_H
#define LIBFIX_H
#define LIBFIX_API extern
LIBFIX_API int fix_add (int a, int b);
LIBFIX_API int fix_mul (int a, int b);
#define fix_twice(x) fix_add((x), (x))
#endif
