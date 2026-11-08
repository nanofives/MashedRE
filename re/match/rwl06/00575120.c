// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x00575120: hand-matched (h2). 69.3% regs 73.3% ops 84.0% under FLAGS /O2 /Ob1 /G5 (raw file was /O1 at 6.5%). Float abs sums via (float)fabs, <= compares, entry stores as 32-bit integer copies through an entry struct. Residual: original keeps two separate 'return 0' epilogues and builds esi=entry+0xc as a second base (push edi), plus an fxch in the second abs sum; ours merges the returns. Class b. */
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
typedef unsigned int uint;
MGLOBAL(float, DAT_005d757c, 0x005d757c);
double fabs(double x);
#pragma intrinsic(fabs)
typedef struct { char pad[0xc]; undefined4 x; undefined4 y; undefined4 z; undefined4 a; undefined4 b; } E;
// MATCH: 0x00575120 FUN_00575120
undefined4 FUN_00575120(float *param_1,undefined4 param_2,int param_3,float param_4)
{
  int iVar1;
  uint uVar2;
  float fVar3;
  E *e;

  uVar2 = *(uint *)(param_3 + 0xac);
  if ((uVar2 != 0) && (DAT_005d757c <= param_4)) {
    fVar3 = (float)fabs(*(float *)(param_3 + 0x14) - param_1[2]) +
            (float)fabs(*(float *)(param_3 + 0x10) - param_1[1]) +
            (float)fabs(*(float *)(param_3 + 0xc) - *param_1);
    if (fVar3 <= param_4) {
      return 0;
    }
    if (1 < uVar2) {
      iVar1 = param_3 + uVar2 * 0x28;
      fVar3 = (float)fabs(*(float *)(iVar1 + -0x1c) - *param_1) +
              (float)fabs(*(float *)(iVar1 + -0x18) - param_1[1]) +
              (float)fabs(*(float *)(iVar1 + -0x14) - param_1[2]);
      if (fVar3 <= param_4) {
      return 0;
    }
    }
  }
  e = (E *)(param_3 + uVar2 * 0x28);
  e->x = *(undefined4 *)param_1;
  e->y = *(undefined4 *)(param_1 + 1);
  e->z = *(undefined4 *)(param_1 + 2);
  e->b = 0;
  e->a = param_2;
  *(int *)(param_3 + 0xac) = *(int *)(param_3 + 0xac) + 1;
  return 1;
}
