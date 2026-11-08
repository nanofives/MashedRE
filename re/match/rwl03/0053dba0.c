// FLAGS: /O2 /Ob1 /G5
// h3: 66.2% regs 76.0% (raw converter form with cxx fixes: code typedef arity and pointer prototypes, DDK score unchanged).
/* MATCH-RWL03 0x0053dba0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
#ifdef __cplusplus
typedef int code(int, int);
#else
typedef int code();
#endif
#define CONCAT22(a,b) ((uint)(((uint)(ushort)(a)<<16)|(ushort)(b)))
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
void FUN_004c1800(int * p0, int p1);
float * FUN_004c1840(int * p0, int p1);
int FUN_0053d5a0(ushort p0, ushort p1, int * p2, uint p3);
void FUN_0053de50(int * p0);
undefined4 * FUN_0053dea0(int * p0);
void FUN_0053e790(int * p0);
int FUN_0053e7f0(int * p0);
// MATCH: 0x0053dba0 FUN_0053dba0
int FUN_0053dba0(int param_1,int param_2,int param_3,int param_4,int param_5)

{
  byte *pbVar1;
  short sVar2;
  int *piVar3;
  int iVar4;
  int *piVar5;
  uint uVar6;
  byte *pbVar7;
  undefined2 extraout_var;
  int local_118;
  int iStack_114;
  int iStack_110;
  int *piStack_10c;
  int *apiStack_108 [66];
  
  local_118 = 0;
  piVar3 = (int *)(**(code **)(DAT_007d3ff8 + 0x108))(0x3c,0);
  if (piVar3 != (int *)0x0) {
    iVar4 = (**(code **)(DAT_007d3ff8 + 0x108))(param_1 * 0xc,0);
    piVar3[1] = iVar4;
    if (iVar4 != 0) {
      iVar4 = (**(code **)(DAT_007d3ff8 + 0x108))(param_1 * 4,0);
      piVar3[0xe] = iVar4;
      if (iVar4 != 0) {
        iVar4 = (**(code **)(DAT_007d3ff8 + 0x108))(param_3 * 8,0);
        piVar3[2] = iVar4;
        if (iVar4 != 0) {
          piVar3[0xc] = 0;
          piVar3[4] = 0;
          piVar3[0xb] = param_3;
          *piVar3 = param_1;
          piVar3[3] = param_4;
          FUN_004c1800(piVar3 + 5,param_2);
          if (0 < param_1) {
            iVar4 = 0;
            do {
              FUN_004c1840(piVar3 + 5,param_2);
              *(int *)(iVar4 + piVar3[1]) = param_2;
              iVar4 = iVar4 + 0xc;
              param_2 = param_2 + 0xc;
              param_1 = param_1 + -1;
            } while (param_1 != 0);
          }
          iVar4 = 0;
          if (0 < param_3) {
            do {
              *(int *)(piVar3[2] + iVar4 * 8) = param_4 + iVar4 * 8;
              iVar4 = iVar4 + 1;
            } while (iVar4 < param_3);
          }
          piVar5 = (int *)FUN_0053dea0(piVar3);
          piStack_10c = piVar5;
          if (piVar5 != (int *)0x0) {
            iVar4 = FUN_0053e7f0(piVar5);
            iStack_110 = FUN_0053d5a0(CONCAT22(extraout_var,(short)piVar3[0xb]),iVar4 + -1,
                                      piVar3 + 5,param_5 == 0);
            iVar4 = 0;
            if (iStack_110 == 0) {
              local_118 = 0;
            }
            else {
              if (param_5 == 0) {
                param_5 = *(int *)(iStack_110 + 0x24);
              }
              uVar6 = 0;
              if (piVar3[0xb] != 0) {
                do {
                  *(short *)(param_5 + uVar6 * 2) =
                       (short)(*(int *)(piVar3[2] + uVar6 * 8) - piVar3[3] >> 3);
                  uVar6 = uVar6 + 1;
                } while (uVar6 < (uint)piVar3[0xb]);
              }
              local_118 = iStack_110;
              if (-1 < *piVar5) {
                sVar2 = 0;
                apiStack_108[0] = &local_118;
                apiStack_108[1] = piVar5;
                iStack_114 = 0;
                do {
                  pbVar7 = (byte *)(*(int *)(iStack_110 + 0x20) + iStack_114);
                  pbVar1 = (byte *)apiStack_108[iVar4 * 2 + 1];
                  *(short *)apiStack_108[iVar4 * 2] = sVar2;
                  pbVar7[8] = *pbVar1;
                  *pbVar7 = *pbVar1 | 1;
                  *(undefined4 *)(pbVar7 + 4) = *(undefined4 *)(pbVar1 + 0xc);
                  *(undefined4 *)(pbVar7 + 0xc) = *(undefined4 *)(pbVar1 + 0x10);
                  piVar5 = *(int **)(pbVar1 + 8);
                  if (*piVar5 < 0) {
                    pbVar7[9] = *(byte *)(piVar5 + 1);
                    *(undefined2 *)(pbVar7 + 10) = *(undefined2 *)((int)piVar5 + 6);
                  }
                  else {
                    pbVar7[9] = 0xff;
                    apiStack_108[iVar4 * 2 + 1] = *(int **)(pbVar1 + 8);
                    apiStack_108[iVar4 * 2] = (int *)(pbVar7 + 10);
                    iVar4 = iVar4 + 1;
                  }
                  piVar5 = *(int **)(pbVar1 + 4);
                  if (*piVar5 < 0) {
                    pbVar7[1] = *(byte *)(piVar5 + 1);
                    *(undefined2 *)(pbVar7 + 2) = *(undefined2 *)((int)piVar5 + 6);
                  }
                  else {
                    pbVar7[1] = 0xff;
                    apiStack_108[iVar4 * 2 + 1] = *(int **)(pbVar1 + 4);
                    apiStack_108[iVar4 * 2] = (int *)(pbVar7 + 2);
                    iVar4 = iVar4 + 1;
                  }
                  sVar2 = sVar2 + 1;
                  iStack_114 = iStack_114 + 0x10;
                  iVar4 = iVar4 + -1;
                  piVar5 = piStack_10c;
                } while (-1 < iVar4);
              }
            }
            FUN_0053de50(piVar5);
          }
          FUN_0053e790(piVar3);
          return local_118;
        }
        FUN_0053e790(piVar3);
        return local_118;
      }
    }
    FUN_0053e790(piVar3);
  }
  return 0;
}
