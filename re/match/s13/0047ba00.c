// FLAGS: /O2 /GS /Ob1
/* 0x0047ba00 (calls FUN_00496940; original pushes its one stack arg, call, add esp,4, ret): 0% (regs 0%).
   FUN_00496940 takes no args (re/match/s16/00496940.c on orch/MATCH-S16, `int (void)`). Every form tried
   compiles to the 5-byte tail jmp (with or without forwarding the arg): typed/K&R/varargs/(void) callee
   prototypes, int/void/float wrapper returns, address-taken param (*p, p[0], p[i]), varargs wrapper.
   Only a __stdcall callee (80%, ret differs) or an 8-byte arg (wrong arg size) stop it. The original pushes
   the arg, so the call site passes a; the callee ignores it. Kept the void-callee form. Class (b). */
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
int FUN_00496940(void);
// MATCH: 0x0047ba00 FUN_0047ba00
void FUN_0047ba00(int a)
{
  FUN_00496940();
}
