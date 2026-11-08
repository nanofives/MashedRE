// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x00559560: hand-matched (h3). 68.3% (regs 68.3, ops 87.8). residue: original keeps `(x+0x1f)>>5<<5` as shr/shl, ours folds it to `and ~0x1f` for every spelling tried, and the lea chain order differs. class (a). */
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
int FUN_00563de0(int p0, int p1);
// MATCH: 0x00559560 FUN_00559560
int FUN_00559560(int param_1)
{
  int iVar1;
  int iVar2;

  iVar2 = *(int *)(param_1 + 0x14);
  iVar1 = FUN_00563de0(*(int *)(param_1 + 8), *(int *)(param_1 + 0x1c));
  iVar2 = iVar1 + 0xc11c + iVar2 * 0x148;
  iVar2 = *(int *)(param_1 + 0x14) * 0x30 + iVar2 +
          ((((unsigned int)*(int *)(param_1 + 8) + 0x1f) >> 5) << 5);
  if (*(int *)(param_1 + 0x18) != 0) {
    iVar2 = iVar2 + *(int *)(param_1 + 0x18) * 0xa0;
  }
  iVar1 = *(int *)(param_1 + 0xc);
  if (iVar1 != 0) {
    iVar2 = iVar2 + ((((unsigned int)iVar1 + 0x1f) >> 5) + iVar1 * 4) * 4;
  }
  return iVar2 + *(int *)(param_1 + 8) * 0xc;
}
