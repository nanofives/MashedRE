// FLAGS: /O2 /Ob1 /G5
// MATCH-S27 0x005bac00: 59.0% (regs 67%). Ghidra's 3-call body was wrong: the real function initialises a 12-dword block and runs three QueryInterface-style vtable calls (+0x14) at IIDs 0x005e70e0/0x005e70c0/0x005e7080, each followed by FUN_005baa60(self, k, block) on param_1 == 3, returning k (1/4/2). Param slot is the out variable. Residual: compiler hoists self+8 / self into esi+edi, original reloads [esi+8] each call; volatile and local-copy forms tried. Class: b.
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
typedef unsigned char undefined1;
typedef int (__stdcall *QFN)(void *, void *, int, void *);
MGLOBAL(undefined1, DAT_005e70e0, 0x005e70e0);
MGLOBAL(undefined1, DAT_005e70c0, 0x005e70c0);
MGLOBAL(undefined1, DAT_005e7080, 0x005e7080);
undefined4 FUN_005baa60(int p0, int p1, int *p2);
// MATCH: 0x005bac00 FUN_005bac00
undefined4 FUN_005bac00(int param_1)
{
  int local[12];
  int self;

  self = param_1;
  local[0] = -10000;
  local[1] = -10000;
  local[2] = 0;
  local[3] = 0x3fbeb852;
  local[4] = 0x3f547ae1;
  local[5] = -2602;
  local[6] = 0x3be56042;
  local[7] = 200;
  local[8] = 0x3c343958;
  local[9] = 0x42c80000;
  local[10] = 0x42c80000;
  local[11] = 0x459c4000;
  if (((QFN)*(int *)(**(int **)(self + 8) + 0x14))(*(int **)(self + 8), &DAT_005e70e0, 0, &param_1) >= 0 && param_1 == 3 &&
      FUN_005baa60(self, 1, local) != 0) {
    return 1;
  }
  if (((QFN)*(int *)(**(int **)(self + 8) + 0x14))(*(int **)(self + 8), &DAT_005e70c0, 1, &param_1) >= 0 && param_1 == 3 &&
      FUN_005baa60(self, 4, local) != 0) {
    return 4;
  }
  if (((QFN)*(int *)(**(int **)(self + 8) + 0x14))(*(int **)(self + 8), &DAT_005e7080, 0, &param_1) >= 0 && param_1 == 3 &&
      FUN_005baa60(self, 2, local) != 0) {
    return 2;
  }
  return 0;
}
