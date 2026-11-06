// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 (h1) 0x00484170: 100% (regs 100%). Hand: 0x18-byte record struct, allocator through the
   RwGlobals slot +0x108, divisor loaded into a local first (gives fld x; fdivr [0x5cc320] = 1.0f global;
   inline division 91.7%). No residual. */
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
typedef void *(*MemAllocFn)(int size, int hint);
typedef struct { int a; int b; int c; float inv; int d; int e; } Rec18;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
// MATCH: 0x00484170 FUN_00484170
Rec18 *FUN_00484170(int param_1, int *param_2, int *param_3)
{
  float t;
  Rec18 *r = (Rec18 *)(*(MemAllocFn *)(DAT_007d3ff8 + 0x108))(0x18, 0);
  if (r != 0) {
    r->a = param_3[0];
    r->b = param_3[1];
    r->c = *param_2;
    t = *(float *)(param_1 + 0x80);
    r->inv = DAT_005cc320 / t;
    r->d = 0;
    r->e = 0;
    return r;
  }
  return 0;
}
