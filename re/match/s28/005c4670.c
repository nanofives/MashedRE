// FLAGS: /O2 /Ob1 /G5
// best 68.2 (regs 87.5, ops 94.3). Hand-laid frame: one uint l[11] (type @0, error rec @1..2, 8-dword zeroed block @3..10) reproduces the 0x2c frame. Residual: prologue/arg scheduling (original loads stream into ebx before pushes, takes the lea args first) and call-argument push order; permuter variant (89.7) reordered the error-record write after the call (semantic change), rejected. Class (b).
/* MATCH-S28 0x005c4670: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
MGLOBAL(undefined4, DAT_00617f98, 0x00617f98);
int FUN_004c1a10(int p0, undefined4 * p1);
int FUN_004c1a40(int p0, undefined4 p1);
int FUN_004c1b10(int p0, undefined4 p1);
int FUN_004c1c10(int p0, int p1);
int FUN_004c1c80(int p0, undefined4 * p1);
undefined1 * FUN_004c1d30(void);
uint FUN_004cbd30(undefined4 * p0, undefined4 * p1, uint p2);
undefined4 FUN_004cc5e0(undefined4 p0, int p1, undefined4 * p2, uint * p3);
undefined4 FUN_004d7ff0(undefined4 p0);
undefined4 * FUN_004d8480(undefined4 * p0);
int FUN_004e1b60(int p0, int p1, int p2);
// MATCH: 0x005c4670 FUN_005c4670
int FUN_005c4670(int param_1)
{
  int iVar1;
  int iVar2;
  undefined4 *puVar3;
  uint l[11];

  iVar2 = param_1;
  iVar1 = FUN_004cc5e0(param_1, 1, (undefined4 *)&param_1, &l[0]);
  if (iVar1 != 0) {
    if ((l[0] < 0x35000) || (0x37002 < l[0])) {
      l[1] = 1;
      l[2] = FUN_004d7ff0(0x80000004);
      FUN_004d8480(&l[1]);
    }
    else {
      puVar3 = &l[3];
      for (iVar1 = 8; iVar1 != 0; iVar1 = iVar1 + -1) {
        *puVar3 = 0;
        puVar3 = puVar3 + 1;
      }
      iVar1 = FUN_004cbd30((undefined4 *)iVar2, &l[3], param_1);
      if (iVar1 == param_1) {
        iVar1 = (int)FUN_004c1d30();
        if (iVar1 != 0) {
          iVar2 = FUN_004e1b60((int)&DAT_00617f98, iVar2, iVar1);
          if (iVar2 != 0) {
            FUN_004c1c80(iVar1, &l[3]);
            FUN_004c1a10(iVar1, &l[5]);
            FUN_004c1a40(iVar1, l[7]);
            FUN_004c1b10(iVar1, l[8]);
            *(undefined4 *)(iVar1 + 0x88) = l[9];
            FUN_004c1c10(iVar1, l[10]);
            return iVar1;
          }
        }
      }
    }
  }
  return 0;
}
