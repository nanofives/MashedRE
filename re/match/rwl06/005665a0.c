// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x005665a0: hand pass (h2), body kept in raw form. 27.8% regs 40.7% ops 64.6% under /O2 /Ob1 /G5. Compiles after: 0x00566420 prototype (returns float, 7 args), float fPrev in place of Ghidra's param_5-as-float reuse, DAT_005d757c float. Structure (loops, branches) from the decomp unchanged; the original keeps different counters in registers (ebp/edi) and does not spill the same temps. Not iterated beyond the compile fix (budget). Class b. */
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
typedef long double float10;
MGLOBAL(float, DAT_005d757c, 0x005d757c);
float FUN_00566420(float p0, float p1, float p2, float p3, int p4, float * p5, float * p6);
// MATCH: 0x005665a0 FUN_005665a0
int FUN_005665a0(float *param_1,float param_2,float param_3,int param_4,float *param_5)

{
  int iVar1;
  float fVar2;
  float fVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  float *pfVar7;
  float *pfVar8;
  float fVar9;
  float fPrev;
  int local_14;
  float local_c;
  int local_8;
  
  if (param_4 != 0) {
    while (param_5[param_4] == DAT_005d757c) {
      param_4 = param_4 + -1;
      if (param_4 == 0) {
        return 0;
      }
    }
    if (param_4 != 0) {
      iVar6 = param_4 + -1;
      pfVar7 = param_5;
      iVar5 = iVar6;
      if (0 < iVar6) {
        do {
          param_5 = pfVar7 + param_4 + 1;
          iVar4 = 0;
          if (-1 < iVar5) {
            do {
              pfVar7 = pfVar7 + 1;
              iVar1 = iVar4 + 1;
              param_5[iVar4] = (float)iVar1 * *pfVar7;
              iVar4 = iVar1;
            } while (iVar1 <= iVar5);
          }
          iVar6 = iVar6 + -1;
          pfVar7 = param_5;
          iVar5 = iVar5 + -1;
        } while (iVar6 != 0);
      }
      local_14 = 0;
      fVar2 = -(*param_5 / param_5[1]);
      if ((param_2 <= fVar2) && (fVar2 < param_3 != (fVar2 == param_3))) {
        *param_1 = fVar2;
        local_14 = 1;
      }
      iVar6 = 2;
      pfVar7 = param_5;
      if (1 < param_4) {
        do {
          pfVar8 = (float *)((int)pfVar7 - (param_4 * 4 + 4));
          iVar4 = 0;
          local_c = param_2;
          fPrev = pfVar8[iVar6];
          iVar5 = iVar6;
          while (iVar5 != 0) {
            iVar5 = iVar5 + -1;
            fPrev = fPrev * param_2 + pfVar8[iVar5];
          }
          local_8 = 0;
          if (0 < local_14) {
            do {
              fVar2 = param_1[local_8];
              fVar3 = pfVar8[iVar6];
              iVar5 = iVar6;
              while (iVar5 != 0) {
                iVar5 = iVar5 + -1;
                fVar3 = fVar3 * fVar2 + pfVar8[iVar5];
              }
              if (fVar3 == DAT_005d757c) {
                param_1[iVar4] = fVar2;
LAB_00566733:
                iVar4 = iVar4 + 1;
              }
              else if (fPrev * fVar3 < DAT_005d757c) {
                fVar9 = FUN_00566420(local_c,fVar2,fPrev,fVar3,iVar6,pfVar8,pfVar7);
                param_1[iVar4] = (float)fVar9;
                goto LAB_00566733;
              }
              local_8 = local_8 + 1;
              fPrev = fVar3;
              local_c = fVar2;
            } while (local_8 < local_14);
          }
          fVar2 = pfVar8[iVar6];
          iVar5 = iVar6;
          while (iVar5 != 0) {
            iVar5 = iVar5 + -1;
            fVar2 = fVar2 * param_3 + pfVar8[iVar5];
          }
          if (fVar2 * fPrev < DAT_005d757c) {
            fVar9 = FUN_00566420(local_c,param_3,fPrev,fVar2,iVar6,pfVar8,pfVar7);
            param_1[iVar4] = (float)fVar9;
            iVar4 = iVar4 + 1;
          }
          iVar6 = iVar6 + 1;
          pfVar7 = pfVar8;
          local_14 = iVar4;
        } while (iVar6 <= param_4);
      }
      return local_14;
    }
  }
  return 0;
}
