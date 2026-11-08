// FLAGS: /O2 /GS /Ob1
/* MATCH-S18 0x004b5360 (h3): hand, 100%. 5-byte jmp thunk to FUN_005507b0; target arity from its own body:
   2 args ([esp+0x78], [esp+0x7c] after sub 0x64 + 4 pushes), plain ret. g2s scored 0% / compile-fail (Ghidra sizes the thunk as one instruction). */
/* ---- MATCH-S11 preamble (same text in every re/match/s11 source; not part of any function body) ----
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
void * FUN_005507b0(const char *a1, const char *a2);
// MATCH: 0x004b5360 FUN_004b5360
void * FUN_004b5360(const char *a1, const char *a2)
{
  return FUN_005507b0(a1, a2);
}
