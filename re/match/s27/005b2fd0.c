// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 0x005b2fd0: 100.0% (regs 100.0%). Hand: S/EB structs, EB* e = &s->b[j] used twice (divisor reload) - the pointer form fixed the lea ordering; v reused for the remainder. */
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
typedef unsigned int uint;
typedef struct EB { char pad[0x20]; uint f20; char pad2[4]; } EB;
typedef struct S { char pad0[0x10]; char *a; int pad14; EB *b; int pad1c; int f20; } S;
// MATCH: 0x005b2fd0 FUN_005b2fd0
int FUN_005b2fd0(S *s, uint v, int i, int j, int *out)
{
  EB *e = &s->b[j];
  uint q = v / e->f20;
  v = v - e->f20 * q;
  *out = v;
  return *(int *)(s->a + 0x1c + i * 0x20) + s->f20 * q + v;
}
