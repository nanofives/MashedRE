// FLAGS: /O2 /GS /Ob1
/* MATCH-RWL03 0x00532b80: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned char undefined1;
typedef unsigned int uint;
typedef long double float10;
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(undefined1, DAT_005cc320, 0x005cc320);
MGLOBAL(undefined1, DAT_005cea88, 0x005cea88);
MGLOBAL(undefined1, DAT_005cc908, 0x005cc908);
MGLOBAL(undefined1, DAT_005cc904, 0x005cc904);
MGLOBAL(undefined1, DAT_005cc900, 0x005cc900);
MGLOBAL(undefined1, DAT_005cc8fc, 0x005cc8fc);
MGLOBAL(undefined1, DAT_005cc8f8, 0x005cc8f8);
MGLOBAL(undefined1, DAT_005cc8f4, 0x005cc8f4);
MGLOBAL(undefined1, DAT_005ce2f4, 0x005ce2f4);
MGLOBAL(undefined1, DAT_005cd108, 0x005cd108);
MGLOBAL(undefined1, DAT_005d8dec, 0x005d8dec);
MGLOBAL(undefined1, DAT_005d8de8, 0x005d8de8);
MGLOBAL(undefined1, DAT_005d8de4, 0x005d8de4);
MGLOBAL(undefined1, DAT_005d8de0, 0x005d8de0);
MGLOBAL(undefined1, DAT_005d8ddc, 0x005d8ddc);
MGLOBAL(undefined1, DAT_005d8dd8, 0x005d8dd8);
MGLOBAL(undefined1, DAT_005d8dd4, 0x005d8dd4);
MGLOBAL(undefined1, DAT_005d8dd0, 0x005d8dd0);
MGLOBAL(undefined1, DAT_005d8dcc, 0x005d8dcc);
MGLOBAL(undefined1, DAT_005d8dc8, 0x005d8dc8);
MGLOBAL(undefined1, DAT_005d8dc4, 0x005d8dc4);
MGLOBAL(undefined1, DAT_005cc32c, 0x005cc32c);
MGLOBAL(undefined1, DAT_005d8dc0, 0x005d8dc0);
float10 FUN_004c3b30(float p0);
MGLOBAL(int, _DAT_005cc320, 0x005cc320);
MGLOBAL(int, _DAT_005cea88, 0x005cea88);
MGLOBAL(int, _DAT_005cd108, 0x005cd108);
MGLOBAL(int, _DAT_005d8dc4, 0x005d8dc4);
MGLOBAL(int, _DAT_005d8dc8, 0x005d8dc8);
MGLOBAL(int, _DAT_005d8dec, 0x005d8dec);
MGLOBAL(int, _DAT_005d8de8, 0x005d8de8);
MGLOBAL(int, _DAT_005d8de4, 0x005d8de4);
MGLOBAL(int, _DAT_005d8de0, 0x005d8de0);
MGLOBAL(int, _DAT_005d8ddc, 0x005d8ddc);
MGLOBAL(int, _DAT_005cc8f4, 0x005cc8f4);
MGLOBAL(int, _DAT_005d8dd8, 0x005d8dd8);
MGLOBAL(int, _DAT_005d8dd4, 0x005d8dd4);
MGLOBAL(int, _DAT_005d8dd0, 0x005d8dd0);
MGLOBAL(int, _DAT_005d8dcc, 0x005d8dcc);
MGLOBAL(int, _DAT_005cc32c, 0x005cc32c);
MGLOBAL(int, _DAT_005d8dc0, 0x005d8dc0);
MGLOBAL(int, _DAT_005ce2f4, 0x005ce2f4);
MGLOBAL(int, _DAT_005cc908, 0x005cc908);
MGLOBAL(int, _DAT_005cc904, 0x005cc904);
MGLOBAL(int, _DAT_005cc900, 0x005cc900);
MGLOBAL(int, _DAT_005cc8fc, 0x005cc8fc);
MGLOBAL(int, _DAT_005cc8f8, 0x005cc8f8);
// MATCH: 0x00532b80 FUN_00532b80
/* [C1 2026-05-20] Quaternion SLERP plus linear translation. */

void FUN_00532b80(int param_1,float param_2,int param_3,float param_4)

{
  float fVar1;
  float fVar2;
  int iVar3;
  float fVar4;
  float10 fVar5;
  float10 fVar6;
  float10 fVar7;
  float10 fVar8;
  
  iVar3 = (int)param_2;
  fVar8 = (float10)*(float *)((int)param_2 + 0x14) * (float10)*(float *)(param_3 + 0x14) +
          (float10)*(float *)(param_3 + 0x10) * (float10)*(float *)((int)param_2 + 0x10) +
          (float10)*(float *)((int)param_2 + 0xc) * (float10)*(float *)(param_3 + 0xc) +
          (float10)*(float *)(param_3 + 8) * (float10)*(float *)((int)param_2 + 8);
  fVar2 = (param_4 - *(float *)((int)param_2 + 4)) /
          (*(float *)(param_3 + 4) - *(float *)((int)param_2 + 4));
  *(float *)(param_1 + 0x18) =
       (*(float *)(param_3 + 0x18) - *(float *)((int)param_2 + 0x18)) * fVar2 +
       *(float *)((int)param_2 + 0x18);
  *(float *)(param_1 + 0x1c) =
       (*(float *)(param_3 + 0x1c) - *(float *)((int)param_2 + 0x1c)) * fVar2 +
       *(float *)((int)param_2 + 0x1c);
  *(float *)(param_1 + 0x20) =
       (*(float *)(param_3 + 0x20) - *(float *)((int)param_2 + 0x20)) * fVar2 +
       *(float *)((int)param_2 + 0x20);
  if (fVar8 < (float10)DAT_005d757c) {
    fVar8 = -fVar8;
    *(float *)(param_3 + 8) = -*(float *)(param_3 + 8);
    *(float *)(param_3 + 0xc) = -*(float *)(param_3 + 0xc);
    *(float *)(param_3 + 0x10) = -*(float *)(param_3 + 0x10);
    *(float *)(param_3 + 0x14) = -*(float *)(param_3 + 0x14);
  }
  param_4 = _DAT_005cc320 - fVar2;
  param_2 = fVar2;
  if (fVar8 < (float10)_DAT_005cea88) {
    fVar1 = (float)fVar8;
    fVar4 = ABS(fVar1);
    if ((uint)fVar4 < 0x3f800000) {
      if ((uint)fVar4 < 0x3f000000) {
        if ((uint)fVar4 < 0x23000001) {
          fVar8 = (float10)_DAT_005cd108;
        }
        else {
          fVar5 = fVar8 * fVar8;
          fVar8 = (float10)_DAT_005d8dc4 -
                  (fVar8 - ((float10)_DAT_005d8dc8 -
                           ((((((((float10)_DAT_005d8dec * fVar5 + (float10)_DAT_005d8de8) * fVar5 -
                                (float10)_DAT_005d8de4) * fVar5 + (float10)_DAT_005d8de0) * fVar5 -
                              (float10)_DAT_005d8ddc) * fVar5 + (float10)_DAT_005cc8f4) * fVar5) /
                           ((((fVar5 * (float10)_DAT_005d8dd8 - (float10)_DAT_005d8dd4) * fVar5 +
                             (float10)_DAT_005d8dd0) * fVar5 - (float10)_DAT_005d8dcc) * fVar5 +
                           (float10)_DAT_005cc320)) * fVar8));
        }
      }
      else if ((int)fVar1 < 0) {
        fVar1 = (float)((fVar8 + (float10)_DAT_005cc320) * (float10)_DAT_005cc32c);
        fVar8 = (float10)FUN_004c3b30(fVar1);
        fVar8 = (((((((((float10)fVar1 * (float10)_DAT_005d8dec + (float10)_DAT_005d8de8) *
                       (float10)fVar1 - (float10)_DAT_005d8de4) * (float10)fVar1 +
                     (float10)_DAT_005d8de0) * (float10)fVar1 - (float10)_DAT_005d8ddc) *
                    (float10)fVar1 + (float10)_DAT_005cc8f4) * (float10)fVar1) /
                 (((((float10)fVar1 * (float10)_DAT_005d8dd8 - (float10)_DAT_005d8dd4) *
                    (float10)fVar1 + (float10)_DAT_005d8dd0) * (float10)fVar1 -
                  (float10)_DAT_005d8dcc) * (float10)fVar1 + (float10)_DAT_005cc320)) * fVar8 -
                (float10)_DAT_005d8dc8) + fVar8;
        fVar8 = (float10)_DAT_005d8dc0 - (fVar8 + fVar8);
      }
      else {
        fVar1 = (float)(((float10)_DAT_005cc320 - fVar8) * (float10)_DAT_005cc32c);
        fVar8 = (float10)FUN_004c3b30(fVar1);
        fVar4 = (float)((uint)(float)fVar8 & 0xfffff000);
        fVar8 = ((float10)fVar1 - (float10)fVar4 * (float10)fVar4) / ((float10)fVar4 + fVar8) +
                ((((((((float10)fVar1 * (float10)_DAT_005d8dec + (float10)_DAT_005d8de8) *
                      (float10)fVar1 - (float10)_DAT_005d8de4) * (float10)fVar1 +
                    (float10)_DAT_005d8de0) * (float10)fVar1 - (float10)_DAT_005d8ddc) *
                   (float10)fVar1 + (float10)_DAT_005cc8f4) * (float10)fVar1) /
                (((((float10)fVar1 * (float10)_DAT_005d8dd8 - (float10)_DAT_005d8dd4) *
                   (float10)fVar1 + (float10)_DAT_005d8dd0) * (float10)fVar1 -
                 (float10)_DAT_005d8dcc) * (float10)fVar1 + (float10)_DAT_005cc320)) * fVar8 +
                (float10)fVar4;
        fVar8 = fVar8 + fVar8;
      }
    }
    else if ((int)fVar1 < 1) {
      fVar8 = (float10)_DAT_005ce2f4;
    }
    else {
      fVar8 = (float10)DAT_005d757c;
    }
    fVar5 = fVar8 * fVar8;
    fVar5 = (float10)_DAT_005cc320 /
            (fVar5 * fVar8 *
             ((((((float10)_DAT_005cc908 * fVar5 - (float10)_DAT_005cc904) * fVar5 +
                (float10)_DAT_005cc900) * fVar5 - (float10)_DAT_005cc8fc) * fVar5 +
              (float10)_DAT_005cc8f8) * fVar5 - (float10)_DAT_005cc8f4) + fVar8);
    fVar6 = fVar8 * (float10)param_4;
    fVar7 = fVar6 * fVar6;
    param_4 = (float)((fVar7 * fVar6 *
                       ((((((float10)_DAT_005cc908 * fVar7 - (float10)_DAT_005cc904) * fVar7 +
                          (float10)_DAT_005cc900) * fVar7 - (float10)_DAT_005cc8fc) * fVar7 +
                        (float10)_DAT_005cc8f8) * fVar7 - (float10)_DAT_005cc8f4) + fVar6) * fVar5);
    fVar8 = fVar8 * (float10)fVar2;
    fVar6 = fVar8 * fVar8;
    param_2 = (float)((fVar6 * fVar8 *
                       ((((((float10)_DAT_005cc908 * fVar6 - (float10)_DAT_005cc904) * fVar6 +
                          (float10)_DAT_005cc900) * fVar6 - (float10)_DAT_005cc8fc) * fVar6 +
                        (float10)_DAT_005cc8f8) * fVar6 - (float10)_DAT_005cc8f4) + fVar8) * fVar5);
  }
  *(float *)(param_1 + 8) = param_4 * *(float *)(iVar3 + 8) + param_2 * *(float *)(param_3 + 8);
  *(float *)(param_1 + 0xc) =
       param_2 * *(float *)(param_3 + 0xc) + param_4 * *(float *)(iVar3 + 0xc);
  *(float *)(param_1 + 0x10) =
       param_4 * *(float *)(iVar3 + 0x10) + param_2 * *(float *)(param_3 + 0x10);
  *(float *)(param_1 + 0x14) =
       param_2 * *(float *)(param_3 + 0x14) + param_4 * *(float *)(iVar3 + 0x14);
  return;
}
