// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x00566ca0: hand-matched (h2). 73.7% regs 73.7% ops 94.7% under FLAGS /O2 /Ob1 /G5 (raw file was /O1 at 30.4%). Same shape as 0x00566c10 (<= spelled fVar3 <= DAT_005d757c, new local for the result, else-branch for the zero case). Residual: second dot product emitted as [2],[1],[0] instead of [1],[0],[2] (no spelling of the sum, a temp, or 6 permutations of the terms moved it) and the final 'return 1' block layout. Permuter 82 compiles. Class b. */
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
typedef unsigned int uint;
MGLOBAL(float, DAT_005d757c, 0x005d757c);
// MATCH: 0x00566ca0 FUN_00566ca0
uint FUN_00566ca0(float *param_1,float *param_2,float *param_3,float *param_4,float param_5)
{
  float fVar1;
  float fVar3;
  uint uVar2;

  fVar3 = (param_2[1] * param_4[1] + *param_2 * *param_4 + param_2[2] * param_4[2]) - param_5;
  uVar2 = (uint)(fVar3 <= DAT_005d757c);
  if (param_1 != (float *)0x0) {
    if (uVar2 == 0) {
      *param_1 = fVar3;
      fVar1 = -(param_3[1] * param_4[1] + *param_3 * *param_4 + param_3[2] * param_4[2]);
      param_1[1] = fVar1;
      if (fVar3 > fVar1) {
        if (fVar1 < DAT_005d757c) {
          return 0xffffffff;
        }
        return 0;
      }
      uVar2 = 1;
    } else {
      *param_1 = 0.0;
      param_1[1] = 1.0;
      return 1;
    }
  }
  return uVar2;
}
