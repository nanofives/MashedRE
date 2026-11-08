// FLAGS: /O2 /Ob1 /G5
// h3: 100% regs 100%. param_2 uint, if/else with the (param_2 & 1) != 0 arm first.
/* MATCH-RWL03 0x004f3b00: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl03_20261108/results.tsv. */
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
typedef int (*Cb4f3b00)(int, int);
MGLOBAL(Cb4f3b00, PTR_FUN_00618a10, 0x00618a10);
MGLOBAL(int, DAT_00618a14, 0x00618a14);
MGLOBAL(undefined1, DAT_00618a00, 0x00618a00);
int FUN_004f36c0(int p0, int p1);
int FUN_004f0bd0(int p0);
// MATCH: 0x004f3b00 FUN_004f3b00
undefined * FUN_004f3b00(int param_1,uint param_2)
{
  Cb4f3b00 pcVar1;
  undefined *puVar2;
  int uVar3;

  if (param_1 != 0) {
    if (*(int *)(param_1 + 4) == 0) {
      FUN_004f0bd0(param_1);
      return &DAT_00618a00;
    }
    if ((param_2 & 1) != 0) {
      pcVar1 = PTR_FUN_00618a10;
      uVar3 = DAT_00618a14;
    }
    else {
      pcVar1 = FUN_004f36c0;
      uVar3 = 0;
    }
    puVar2 = (undefined *)(*pcVar1)(param_1,uVar3);
    if (puVar2 != (undefined *)0x0) {
      FUN_004f0bd0(param_1);
      return puVar2;
    }
  }
  return (undefined *)0x0;
}
