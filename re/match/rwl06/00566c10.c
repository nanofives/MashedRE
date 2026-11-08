// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x00566c10: hand-matched (h2). 88.9% regs 88.9% ops 93.3%. Every instruction matches except the layout of the final 'return 1' (fstp st(0); mov eax,1; ret) block, which the original places after the zero-store block and ours before it. Locals a, b hold the p2/p3 table loads (memory operand loaded first, as in 0x005748a0); new local for the first result (orig reuses param_4's dead slot); gt compare spelled param_6 > fVar1. Tried tail return forms, uVar2=1 after if/else, permuter 105 compiles. Class b. */
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
// MATCH: 0x00566c10 FUN_00566c10
uint FUN_00566c10(float *param_1,int param_2,int param_3,int param_4,float param_5,float param_6)
{
  float fVar1;
  float fVar3;
  float a;
  float b;
  uint uVar2;

  a = *(float *)(param_2 + param_4 * 4);
  fVar3 = a * param_5 - param_6;
  uVar2 = (uint)(fVar3 < DAT_005d757c);
  if (param_1 != (float *)0x0) {
    if (uVar2 == 0) {
      *param_1 = fVar3;
      b = *(float *)(param_3 + param_4 * 4);
      fVar1 = -(b * param_5);
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
