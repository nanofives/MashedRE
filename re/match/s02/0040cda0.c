// FLAGS: /O2 /GS /Ob1
/* 0x0040cda0 (86 B): 100.0% exact, regs 100.0%.
   Raw g2s 57.7%. The three locals are one 12-byte struct passed to RpClumpForAllAtomics
   ({param_2, material, FUN_004672d0(0)}). Last step (permuter): store .material before .a.
   Residual: none. */
/* ---- MATCH-S02 preamble (same text in every re/match/s02 source; not part of any function body) ----
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
typedef struct ShineArgs {
  undefined4 a;
  int material;
  undefined4 b;
} ShineArgs;

MGLOBAL(int, DAT_00636ad0, 0x00636ad0);

void *FUN_0040bb10(void *atomic, void *data);
int FUN_0040bb30(const char *name);
undefined4 FUN_004672d0(int p0);
void *RpClumpForAllAtomics(void *clump, void *(*cb)(void *, void *), void *data);

// MATCH: 0x0040cda0 FUN_0040cda0
void FUN_0040cda0(void *param_1, undefined4 param_2, int param_3)
{
  ShineArgs args;

  if (DAT_00636ad0 == 0) {
    args.b = FUN_004672d0(0);
    if (param_3 == 0) {
      param_3 = FUN_0040bb30("shine");
    }
    args.material = param_3;
    args.a = param_2;
    RpClumpForAllAtomics(param_1, FUN_0040bb10, &args);
  }
}
