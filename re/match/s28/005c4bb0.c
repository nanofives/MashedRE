// FLAGS: /O2 /Ob1 /G5
// 100%. /O2 /Ob1 /G5 (not /O1); signed /4 division, ErrRec 8-byte stack struct, tail p[1]++ after computing the return value.
/* MATCH-S28 0x005c4bb0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs28_20261107/results.tsv. */
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
typedef unsigned int undefined4;
typedef int (*ReallocFn)(int, int, undefined4);
typedef struct { undefined4 a; undefined4 b; } ErrRec;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
undefined4 FUN_004d7ff0(undefined4 p0, int p1);
undefined4 *FUN_004d8480(ErrRec *p0);
// MATCH: 0x005c4bb0 FUN_005c4bb0
int FUN_005c4bb0(int *param_1, undefined4 param_2)
{
  int iVar1;
  int iVar2;
  ErrRec e;

  iVar1 = param_1[2];
  if (param_1[1] >= iVar1) {
    iVar2 = (*(ReallocFn *)(DAT_007d3ff8 + 0x110))(*param_1, (iVar1 / 4 + iVar1) * param_1[3], param_2);
    iVar1 = param_1[2];
    if (iVar2 == 0) {
      e.a = 1;
      e.b = FUN_004d7ff0(0x80000013, (iVar1 / 4 + iVar1) * param_1[3]);
      FUN_004d8480(&e);
      return 0;
    }
    *param_1 = iVar2;
    param_1[2] = iVar1 / 4 + iVar1;
  }
  iVar1 = param_1[1];
  iVar2 = param_1[3] * iVar1 + *param_1;
  param_1[1]++;
  return iVar2;
}
