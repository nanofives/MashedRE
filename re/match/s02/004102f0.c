// FLAGS: /O2 /GS /Ob1
/* 0x004102f0 FUN_004102f0 (172 B): 100.0% exact, regs 100.0%.
   Raw g2s was 74.5% (unaff_EBX). The original takes its one argument in EBX: its only caller FUN_004111c0
   (case 3 of its switch, 0x00411277..0x00411288) does mov ebx,esi with esi = its stack param_1. That is the
   static-function convention (pilot finding 1): FUN_004102f0 is static, defined before a caller in the same file.
   caller_004111c0 is a stand-in for that caller (not matched). Arg is uint (mul 0x88888889 = unsigned /60).
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
MGLOBAL(int, DAT_005f29b8, 0x005f29b8);
MGLOBAL(int *, PTR_PTR_005f2770, 0x005f2770);
MGLOBAL(int, DAT_0063ba8c, 0x0063ba8c);
MGLOBAL(int, DAT_007f0fc8, 0x007f0fc8);

void FUN_00448700(int p0, int p1);
void FUN_004430a0(int p0);
void FUN_0040e590(void);
void FUN_0040d470(int p0);
int FUN_0046c7b0(int p0);
void FUN_0046baa0(int p0);
int FUN_00405460(uint p0);
int FUN_004430b0(void);
void FUN_0040fc00(void);

// MATCH: 0x004102f0 FUN_004102f0
static void FUN_004102f0(uint param_1)
{
  int i;

  if (DAT_005f29b8 == 100000) {
    FUN_00448700(0, 0);
    FUN_004430a0(1);
    FUN_0040e590();
    FUN_0040d470(1);
  }
  for (i = 0; i < 4; i++) {
    if (PTR_PTR_005f2770[i + 0xd] != 0 && FUN_0046c7b0(i) == 1) {
      FUN_0046baa0(i);
    }
  }
  DAT_005f29b8 = DAT_005f29b8 - (param_1 * 0x44c) / 0x3c;
  if (FUN_00405460(param_1) == 0 && FUN_004430b0() == 0) {
    DAT_0063ba8c = 4;
    DAT_005f29b8 = 12000;
  }
}

/* stand-in for the real caller FUN_004111c0 (case 3 of its switch: EBX = ESI = param_1); not matched */
void caller_004111c0(uint param_1)
{
  DAT_007f0fc8 = 0;
  FUN_004102f0(param_1);
  FUN_0040fc00();
}
