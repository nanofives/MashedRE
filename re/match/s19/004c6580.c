// FLAGS: /O2 /GS /Ob1
/* MATCH-S19 0x004c6580: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs19_20261102/results.tsv. */
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
typedef unsigned int uint;
typedef unsigned char byte;
undefined4 * FUN_004cdca0(undefined4 p0, undefined4 p1, undefined4 p2);
undefined4 FUN_004cdd00(uint * p0);
uint * FUN_004cdd60(uint * p0);
void FUN_004d9050(int p0, int p1, undefined4 p2);
int FUN_004d9530(undefined4 * p0, undefined4 * p1, undefined4 * p2);
void FUN_004da4f0(byte * p0, int p1, byte * p2, uint p3, int p4, int p5);
undefined4 FUN_004daa40(int p0);
void FUN_004daaf0(int p0);
void __chkstk(void);
// MATCH: 0x004c6580 FUN_004c6580
/* [C1 2026-05-19] Signature `int f(int* dest_palette, int orig_image, int* mip_array, */

undefined4 FUN_004c6580(int *param_1,int param_2,int *param_3,int param_4,undefined4 param_5)

{
  int *piVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  int *piVar5;
  int iVar6;
  int local_4010;
  undefined1 local_4008 [16388];
  undefined4 uStack_4;
  
  uStack_4 = 0x4c658a;
  piVar5 = *(int **)(*param_3 + 0x18);
  if (piVar5 != (int *)0x0) {
    local_4010 = 1;
    if (1 < param_4) {
      do {
        if ((piVar5 == (int *)0x0) || (*(int *)(param_3[local_4010] + 0x18) == 0)) {
          local_4010 = 0x40;
          break;
        }
        iVar4 = 1 << ((byte)param_5 & 0x1f);
        iVar6 = 0;
        if (0 < iVar4) {
          piVar1 = piVar5;
          do {
            if (*piVar1 !=
                *(int *)((*(int *)(param_3[local_4010] + 0x18) - (int)piVar5) + (int)piVar1)) {
              local_4010 = 0x40;
              break;
            }
            iVar6 = iVar6 + 1;
            piVar1 = piVar1 + 1;
          } while (iVar6 < iVar4);
        }
        local_4010 = local_4010 + 1;
      } while (local_4010 < param_4);
    }
    if (local_4010 == param_4) {
      for (uVar3 = 1 << ((byte)*(undefined4 *)(*param_3 + 0xc) & 0x1f) & 0x3fffffff; uVar3 != 0;
          uVar3 = uVar3 - 1) {
        *param_1 = *piVar5;
        piVar5 = piVar5 + 1;
        param_1 = param_1 + 1;
      }
      for (iVar4 = 0; iVar4 != 0; iVar4 = iVar4 + -1) {
        *(char *)param_1 = (char)*piVar5;
        piVar5 = (int *)((int)piVar5 + 1);
        param_1 = (int *)((int)param_1 + 1);
      }
      return 1;
    }
  }
  iVar4 = FUN_004daa40(local_4008);
  if (iVar4 == 0) {
    return 0;
  }
  iVar4 = 0;
  if (0 < param_4) {
    do {
      FUN_004d9050(local_4008,param_3[iVar4],0x3f800000);
      iVar4 = iVar4 + 1;
    } while (iVar4 < param_4);
  }
  FUN_004d9530(param_1,1 << ((byte)param_5 & 0x1f),local_4008);
  iVar4 = 0;
  if (0 < param_4) {
    do {
      iVar6 = param_3[iVar4];
      iVar2 = FUN_004cdca0(*(undefined4 *)(iVar6 + 4),*(undefined4 *)(iVar6 + 8),param_5);
      if (iVar2 == 0) {
        return 0;
      }
      FUN_004cdd60(iVar2);
      FUN_004da4f0(*(undefined4 *)(iVar2 + 0x14),*(undefined4 *)(iVar2 + 0x10),
                   *(undefined4 *)(iVar2 + 0xc),0,local_4008,iVar6);
      *(int **)(iVar2 + 0x18) = param_1;
      param_3[iVar4] = iVar2;
      if (iVar6 != param_2) {
        FUN_004cdd00(iVar6);
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < param_4);
  }
  FUN_004daaf0(local_4008);
  return 1;
}
