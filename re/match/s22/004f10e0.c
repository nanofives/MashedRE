// FLAGS: /O2 /Ob1 /G5
/* MATCH-S22 0x004f10e0 (h1): best 92.3% (regs 100% renamed, ops 100%, 26/26 ins) under /O2 /Ob1 /G5. Raw 92.3%.
   Size of the record 0x004f0dc0 writes: 12 + 8*n when *param_2 is 8 or 7 with bit 0x1000000 of +8 set, else
   12 + 4*(c + 2*n). Only residual: the original keeps c + 2*n in ecx (lea ecx,[eax+edx*2]; lea eax,[ecx*4+0xc]),
   ours reuses eax. Tried: if/else into a size variable, size variable before the test (23%), unsigned temp, int c
   temp, sizeof(unsigned int) factor, 0xc + 8n + 4c and 0xc + (2n + c)*4 orders; permuter 88 compiles, no gain.
   Class (a): one scratch-register pick no spelling moved. */
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
typedef struct { int a; unsigned short n; short pad; int c; } Obj_004f10e0;
// MATCH: 0x004f10e0 FUN_004f10e0
int FUN_004f10e0(Obj_004f10e0 *p, char *param_2)
{
  if ((*param_2 == 8 && (*(unsigned int *)(param_2 + 8) & 0x1000000)) ||
      (*param_2 == 7 && (*(unsigned int *)(param_2 + 8) & 0x1000000))) {
    return p->n * 8 + 0xc;
  }
  return (p->c + p->n * 2) * 4 + 0xc;
}
