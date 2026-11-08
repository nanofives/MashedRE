// FLAGS: /O2 /Ob1 /G5
/* h2 note: 20.7% raw form. cxx FAIL: FUN_005c4c60 arg1 undefined4 vs int*. Not worked in budget. class (b). */
/* MATCH-RWL04 0x005531f0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl04_20261107/results.tsv. */
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
typedef long double float10;
MGLOBAL(undefined1, DAT_005cc320, 0x005cc320);
float10 FUN_004c3b30(float p0);
float10 FUN_004c3b90(float p0);
undefined4 FUN_0055deb0(int p0);
int FUN_005c4c60(int * p0, int p1, undefined4 p2);
MGLOBAL(int, _DAT_005cc320, 0x005cc320);
// MATCH: 0x005531f0 FUN_005531f0
/* [C1 2026-05-19] 7-param `(path, cx1, cy1, cx2, cy2, x, y)`. Append THREE consecutive segments
   (type=2 each) — a cubic Bezier expressed
 */

int FUN_005531f0(int param_1,float param_2,float param_3,float param_4,float param_5,float param_6,
                float param_7)

{
  float fVar1;
  float fVar2;
  float fVar3;
  int iVar4;
  undefined4 *puVar5;
  float10 fVar6;
  float10 fVar7;
  
  iVar4 = FUN_0055deb0(**(undefined4 **)(param_1 + 0x18));
  puVar5 = (undefined4 *)FUN_005c4c60(**(undefined4 **)(param_1 + 0x18),3,0x30190);
  fVar1 = param_3 - (float)puVar5[-4];
  *puVar5 = 2;
  puVar5[1] = param_2;
  puVar5[2] = param_3;
  param_2 = (float)puVar5[-5] - param_2;
  fVar2 = fVar1 * fVar1 + param_2 * param_2;
  fVar6 = (float10)FUN_004c3b30(fVar2);
  fVar7 = (float10)FUN_004c3b90(fVar2);
  puVar5[3] = (float)((float10)fVar1 * fVar7);
  puVar5[4] = (float)(fVar7 * (float10)param_2);
  puVar5[5] = (float)fVar6 + (float)puVar5[-1];
  if (iVar4 < 2) {
    puVar5[-3] = puVar5[3];
    puVar5[-2] = puVar5[4];
  }
  else {
    fVar1 = (float)puVar5[-3] + (float)((float10)fVar1 * fVar7);
    fVar3 = (float)puVar5[-2] + (float)(fVar7 * (float10)param_2);
    fVar2 = _DAT_005cc320 / ((float)puVar5[-2] * fVar3 + (float)puVar5[-3] * fVar1);
    puVar5[-3] = fVar2 * fVar1;
    puVar5[-2] = fVar2 * fVar3;
  }
  fVar1 = param_5 - (float)puVar5[2];
  puVar5[6] = 2;
  puVar5[7] = param_4;
  param_4 = (float)puVar5[1] - param_4;
  puVar5[8] = param_5;
  fVar2 = fVar1 * fVar1 + param_4 * param_4;
  fVar6 = (float10)FUN_004c3b30(fVar2);
  fVar7 = (float10)FUN_004c3b90(fVar2);
  puVar5[9] = (float)((float10)fVar1 * fVar7);
  puVar5[10] = (float)(fVar7 * (float10)param_4);
  puVar5[0xb] = (float)fVar6 + (float)puVar5[5];
  fVar1 = param_7 - (float)puVar5[8];
  puVar5[0xc] = 2;
  puVar5[0xd] = param_6;
  puVar5[0xe] = param_7;
  param_6 = (float)puVar5[7] - param_6;
  fVar2 = fVar1 * fVar1 + param_6 * param_6;
  fVar6 = (float10)FUN_004c3b30(fVar2);
  fVar7 = (float10)FUN_004c3b90(fVar2);
  puVar5[0xf] = (float)((float10)fVar1 * fVar7);
  puVar5[0x10] = (float)(fVar7 * (float10)param_6);
  puVar5[0x11] = (float)fVar6 + (float)puVar5[0xb];
  *(undefined4 *)(*(int *)(param_1 + 0x18) + 0xc) = 0;
  return param_1;
}
