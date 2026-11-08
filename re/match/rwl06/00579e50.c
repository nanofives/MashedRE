// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWR05 2026-11-08: 83.3% regs 83.3% (was 60.4). Re-derived as struct S {float p[4][6]; float m[4][4]; ... maskA@0x1a4, n@0x1a8, maskB@0x1ac} with a for loop over bit/i and row pointers a, b: register assignment now matches. Residual: the original's strength-reduced row pointer sits at p[i][1] (lea edx,[eax+4]) and emits the dot as z,x,y; 48 term/operand orders in three spellings (array, R struct, explicit a pointer) all compile to one order, x,y,z-anchored at p[i][2]. Class b. Supersedes the header below. */
/* MATCH-RWL06 h1 0x00579e50: 60.4% (regs 87.5%), class b. callee-saved reg assignment ebx/edi swapped. Tried: >=1 compare, term orders (all 12), permuter. cxx:ok. */
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
// MATCH: 0x00579e50 FUN_00579e50
typedef struct { float p[4][6]; float m[4][4]; char pad[0x1a4 - 0xa0]; uint maskA; int n; uint maskB; } S;
void FUN_00579e50(S *s)
{
  int i;
  int bit;
  float *a;
  float *b;

  for (i = 0, bit = 1; bit <= (int)(s->maskA | s->maskB); bit <<= 1, i++) {
    if (bit & (s->maskA | s->maskB)) {
      a = s->p[i];
      b = s->p[s->n];
      s->m[i][s->n] = a[0] * b[0] + a[1] * b[1] + b[2] * a[2];
      s->m[s->n][i] = s->m[i][s->n];
    }
  }
}
