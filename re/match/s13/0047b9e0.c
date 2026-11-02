// FLAGS: /O2 /GS /Ob1
/* 0x0047b9e0 (forwards two stack args to FUN_00496c10; original is push/push/call/add esp,8/ret, not a jmp):
   100% (regs 100%), relocheck OK. The plain typed form `void f(int a, int b) { FUN_00496c10(a, b); }` compiles
   to a bare tail jmp under 9176 (0%), as do varargs/K&R/no-arity callee prototypes, float/short/char* params,
   a volatile local, struct-by-value 4 B. Forms that block the tail call: an address-taken parameter, or an 8 B
   arg (double/__int64/8 B struct by value). Kept the address-taken varargs reading `ap = &a; (ap[0], ap[1])`
   (behaviour-identical on x86 cdecl, reads the two stack args). FUN_00496c10 reads its 2nd arg as a byte.
   Class (b): the real source form that blocks the tail call is not known. */
/* ---- MATCH-S13 preamble (same text in every re/match/s13 source; not part of any function body) ----
   C under the DDK (match.py), C++ under VS2022 when the port includes this file (MATCH_PORT). */
#ifndef MATCH_PREAMBLE
#define MATCH_PREAMBLE
typedef unsigned char undefined; typedef unsigned char undefined1; typedef unsigned short undefined2;
typedef unsigned int undefined3; typedef unsigned int undefined4; typedef unsigned __int64 undefined8;
typedef unsigned int uint; typedef unsigned short ushort; typedef unsigned char uchar; typedef unsigned char byte;
typedef unsigned long ulong; typedef __int64 longlong; typedef unsigned __int64 ulonglong; typedef signed char sbyte;
typedef unsigned short word; typedef unsigned int dword; typedef long double float10;
#ifndef __cplusplus
typedef unsigned char bool;
#endif
#ifdef __cplusplus
/* port: a global is a reference to its original address (the exe maps the original data range) */
#define MGLOBAL(T, n, a) static T &n = *(T *)(a)
#define MGLOBALA(T, n, N, a) static T (&n)[N] = *(T (*)[N])(a)
#else
#define MGLOBAL(T, n, a) extern T n
#define MGLOBALA(T, n, N, a) extern T n[N]
#endif
#endif
/* ---- end preamble ---- */
void FUN_00496c10(int a, int b);
// MATCH: 0x0047b9e0 FUN_0047b9e0
void FUN_0047b9e0(int a, ...)
{
  int *ap;
  ap = &a;
  FUN_00496c10(ap[0], ap[1]);
}
