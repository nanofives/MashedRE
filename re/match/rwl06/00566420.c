// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x00566420: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl06_20261108/results.tsv. */
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
#ifndef __cplusplus
#define true 1
#endif
MGLOBAL(undefined1, DAT_005cc32c, 0x005cc32c);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(undefined1, DAT_005e5470, 0x005e5470);
MGLOBAL(float, _DAT_005cc32c, 0x005cc32c);
MGLOBAL(float, _DAT_005e5470, 0x005e5470);
// MATCH: 0x00566420 FUN_00566420
/* [C1 2026-05-19] Signature: `void FUN_00566420(float p1, float p2, float p3, float p4, int
   param_5, float param_6, int param_7)`. */

void FUN_00566420(float param_1,float param_2,float param_3,float param_4,int param_5,float param_6,
                 int param_7)

{
  float fVar1;
  float fVar2;
  float fVar3;
  int iVar4;
  float fVar5;
  int iVar6;
  int iVar7;
  
  iVar4 = *(int *)&param_6;
  iVar7 = 0x17;
  fVar3 = (param_1 + param_2) * _DAT_005cc32c;
  if ((param_1 <= param_2) && (param_3 * param_4 < DAT_005d757c)) {
    while( true ) {
      fVar1 = *(float *)(iVar4 + param_5 * 4);
      iVar6 = param_5;
      while (iVar6 != 0) {
        iVar6 = iVar6 + -1;
        fVar1 = fVar1 * fVar3 + *(float *)(iVar4 + iVar6 * 4);
      }
      fVar2 = fVar3;
      fVar5 = fVar1;
      if ((DAT_005d757c <= param_3 * fVar1) &&
         (param_1 = fVar3, fVar2 = param_2, param_3 = fVar1, fVar5 = param_4,
         DAT_005d757c <= param_4 * fVar1)) break;
      param_4 = fVar5;
      param_2 = fVar2;
      fVar2 = *(float *)(param_7 + -4 + param_5 * 4);
      iVar6 = param_5 + -1;
      while (iVar6 != 0) {
        iVar6 = iVar6 + -1;
        fVar2 = fVar2 * fVar3 + *(float *)(param_7 + iVar6 * 4);
      }
      if ((DAT_005d757c < (fVar3 - param_1) * fVar2 - fVar1 == DAT_005d757c < fVar2) &&
         (DAT_005d757c < (param_2 - fVar3) * fVar2 + fVar1 == DAT_005d757c < fVar2)) {
        param_6 = fVar3 - fVar1 / fVar2;
      }
      else {
        param_6 = (param_1 + param_2) * _DAT_005cc32c;
      }
      if ((param_6 - fVar3) * (param_6 - fVar3) < param_6 * param_6 * _DAT_005e5470) {
        return;
      }
      iVar7 = iVar7 + -1;
      fVar3 = param_6;
      if (iVar7 == 0) {
        return;
      }
    }
  }
  return;
}
