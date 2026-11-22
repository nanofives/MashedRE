/* Fixture library for the binder's lib kind (re/tools/bind/test_porttop_bind_lib.py, BIND-LIB-ROUTE). Not game code.
   Shapes taken from Lua 4.0's lapi.c: an API macro before the return type, a space before the parameter list, a
   static helper (not linkable: refused), and a definition with its brace on the next line. */
#include "../include/libfix.h"

static int fix_helper (int x) {
  return x + 1;
}

LIBFIX_API int fix_add (int a, int b) {
  return a + b;
}

int
fix_mul (int a,
         int b)
{
  return fix_helper(a * b) - 1;
}

int fix_dup (void) {
  return 1;
}
