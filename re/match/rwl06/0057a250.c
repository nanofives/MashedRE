// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 h3 0x0057a250: best 54.7% (regs 59.0%, ops 75.2%), class (b). v[3]/u[3]/pts[6] arrays, param_4 is an int pointer (fMax float), fabs intrinsic (ABS was an undeclared function), float globals, lim local; permuter swaps of independent statements kept. */
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
typedef unsigned char undefined;
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef long double float10;
MGLOBAL(undefined1, DAT_005cd03c, 0x005cd03c);
MGLOBAL(undefined1, DAT_005ccabc, 0x005ccabc);
MGLOBAL(undefined1, DAT_005cc320, 0x005cc320);
MGLOBAL(undefined1, DAT_005ce1d4, 0x005ce1d4);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(undefined1, DAT_005ce54c, 0x005ce54c);
MGLOBAL(undefined1, DAT_005cc32c, 0x005cc32c);
MGLOBAL(undefined *, PTR_DAT_00623fe8, 0x00623fe8);
float FUN_005667c0(float * p0, float * p1);
undefined4 FUN_00579c00(float * p0, uint p1, int p2, int p3);
float FUN_00579d50(float * p0, float * p1, int p2, int * p3, float * p4);
void FUN_00579e50(int p0);
void FUN_00579ee0(int p0);
MGLOBAL(float, _DAT_005cd03c, 0x005cd03c);
MGLOBAL(float, _DAT_005ccabc, 0x005ccabc);
MGLOBAL(float, _DAT_005cc320, 0x005cc320);
MGLOBAL(float, _DAT_005cc32c, 0x005cc32c);
MGLOBAL(float, _DAT_005ce1d4, 0x005ce1d4);
MGLOBAL(float, _DAT_005ce54c, 0x005ce54c);
double fabs(double);
#pragma intrinsic(fabs)
// MATCH: 0x0057a250 FUN_0057a250
/* [C1 2026-05-20] Signature recovered: `int FUN_0057a250(int body_A, int body_B, float
   depth_cutoff, float
 */

undefined4
FUN_0057a250(undefined4 param_1,undefined4 param_2,float param_3,int param_4,float *param_5,
            float *param_6,float *param_7,float param_8)

{
  int iVar5;
  float *pfVar1;
  float fVar3;
  float fVar2;
  int iVar8;
  uint uVar4;
  uint uVar6;
  unsigned iVar7;
  uint uVar9;
  float fVar10;
  float lim;
  float v[3];
  int local_38;
  float local_40;
  float local_34;
  float pts[6];
  float fMax;
  float u[3];
  
  iVar5 = param_4;
  local_34 = 0.0;
  lim = param_8 * _DAT_005cd03c;
  fVar10 = _DAT_005ccabc;
  uVar9 = 1;
  local_38 = 0;
  *(undefined4 *)(param_4 + 0x1a4) = 0;
  fMax = 0.0;
  v[0] = 0.5;
  v[1] = 0.6;
  v[2] = 0.6244998;
  fVar10 = lim * fVar10 + param_3 + _DAT_005cc320;
  do {
    local_40 = (float)fVar10;
    if (fVar10 <= lim) break;
    *(undefined4 *)(iVar5 + 0x1a8) = 0;
    uVar6 = 1;
    iVar8 = 0;
    *(undefined4 *)(iVar5 + 0x1ac) = 1;
    if ((*(uint *)(iVar5 + 0x1a4) & 1) != 0) {
      do {
        uVar6 = uVar6 * 2;
        iVar8 = iVar8 + 1;
      } while ((*(uint *)(iVar5 + 0x1a4) & uVar6) != 0);
      *(int *)(iVar5 + 0x1a8) = iVar8;
      *(uint *)(iVar5 + 0x1ac) = uVar6;
    }
    pfVar1 = (float *)(iVar5 + *(int *)(iVar5 + 0x1a8) * 0x18);
    fVar2 = (float)fVar10;
    fVar10 = FUN_00579d50(pfVar1,v,0xffffffff,(int *)param_1,(float *)param_2);
    if (fVar10 > param_3) {
      param_5[1] = v[1];
      param_5[2] = v[2];
      FUN_00579c00(v,*(undefined4 *)(iVar5 + 0x1a4),1,iVar5);
      *param_5 = v[0];
      *param_6 = fVar2 * _DAT_005cc32c +
                 param_5[2] * v[2] + v[0] * *param_5 + param_5[1] * v[1];
      *param_7 = fVar2;
      return 2;
    }
    if (fVar2 > fMax) {
      fMax = fVar2;
    }
    if ((local_40 < fMax * _DAT_005ce1d4) ||
       (((DAT_005d757c < local_40 && (DAT_005d757c < fVar2)) &&
        ((local_38 != 0 || (0x28 < (int)uVar9)))))) {
      param_5[2] = v[2];
      param_5[1] = v[1];
      *param_5 = v[0];
      FUN_00579c00(v,*(undefined4 *)(iVar5 + 0x1a4),1,iVar5);
      *param_6 = fVar2 * _DAT_005cc32c +
                 param_5[2] * v[2] + v[0] * *param_5 + param_5[1] * v[1];
      *param_7 = fVar2;
      return 1;
    }
    uVar9 = uVar9 + 1;
    if ((0x32 < (int)uVar9) ||
       ((3 < (int)uVar9 &&
        (((fabs(pfVar1[2] - pts[2]) + fabs(*pfVar1 - pts[0]) + fabs(pfVar1[1] - pts[1])
           < _DAT_005ce54c ||
          (fabs(pfVar1[2] - pts[5]) + fabs(*pfVar1 - pts[3]) + fabs(pfVar1[1] - pts[4]) <
           _DAT_005ce54c)) && (local_38 = local_38 + 1, 5 < local_38)))))) {
      return 0;
    }
    uVar6 = uVar9 & 1;
    fVar2 = pfVar1[1];
    pts[uVar6 * 3] = *pfVar1;
    fVar3 = pfVar1[2];
    pts[uVar6 * 3 + 1] = fVar2;
    pts[uVar6 * 3 + 2] = fVar3;
    FUN_00579e50(iVar5);
    FUN_00579ee0(iVar5);
    uVar6 = 0;
    iVar8 = 0;
    do {
      uVar4 = *(uint *)((&PTR_DAT_00623fe8)[*(int *)(iVar5 + 0x1a4)] + iVar8 * 4);
      if (((iVar8 != 0) || (fMax <= DAT_005d757c)) || ((*(uint *)(iVar5 + 0x1ac) | uVar4) != 0xf)
         ) {
        if (uVar4 == 0) {
          u[0] = *(float *)(iVar5 + *(int *)(iVar5 + 0x1a8) * 0x18);
          iVar7 = iVar5 + *(int *)(iVar5 + 0x1a8) * 0x18;
          u[1] = *(float *)(iVar7 + 4);
          u[2] = *(float *)(iVar7 + 8);
        }
        else {
          iVar7 = FUN_00579c00(u,*(uint *)(iVar5 + 0x1ac) | uVar4,0,iVar5);
          if (iVar7 == 0) goto LAB_0057a521;
        }
        fVar2 = u[2] * u[2] + u[0] * u[0] + u[1] * u[1];
        if ((uVar6 == 0) || (fVar2 < local_34 != (fVar2 == local_34))) {
          v[0] = u[0];
          v[1] = u[1];
          local_34 = fVar2;
          v[2] = u[2];
          uVar6 = uVar4;
        }
      }
LAB_0057a521:
      iVar8 = iVar8 + 1;
    } while (uVar4 != 0);
    fVar10 = FUN_005667c0(v,v);
    uVar6 = *(uint *)(iVar5 + 0x1ac) | uVar6;
    *(uint *)(iVar5 + 0x1a4) = uVar6;
  } while ((int)uVar6 < 0xf);
  *param_7 = -1.0;
  return 1;
}
