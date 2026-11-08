// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x00573670: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned short ushort;
undefined4 FUN_0055abb0(undefined4 * p0, int p1, undefined4 p2);
void FUN_0055bd80(int p0, int p1, undefined4 p2, undefined4 p3);
undefined4 FUN_005729a0(float * p0, int * p1, float * p2, float * p3);
int FUN_00575c60(undefined4 * p0, undefined4 * p1, float * p2, float * p3, float * p4, undefined4 p5);
uint FUN_00576640(int * p0, uint p1, uint p2, int p3, int p4);
// MATCH: 0x00573670 FUN_00573670
int FUN_00573670(int param_1,int param_2,uint param_3,int param_4,int param_5,int param_6,
                int param_7)

{
  ushort uVar1;
  int *piVar2;
  undefined4 uVar3;
  uint uVar4;
  int *piVar5;
  int iVar6;
  int iVar7;
  uint uVar8;
  int *piVar9;
  int local_54;
  int local_50;
  uint local_4c;
  undefined1 local_40 [32];
  undefined1 local_20 [32];
  
  local_54 = 0;
  if ((param_3 != 0) && (param_7 != 0)) {
    local_4c = 0;
    piVar9 = (int *)(param_1 + 0x37c);
    piVar2 = (int *)**(int **)(param_1 + 0x70);
    *(undefined4 *)(param_1 + 0x3ec) = *(undefined4 *)(*piVar2 + 0xc014);
    *(int *)(param_1 + 0x3f0) = piVar2[3];
    uVar3 = *(undefined4 *)(*piVar2 + 0xc018);
    if (param_3 != 0) {
      piVar5 = (int *)(param_2 + 8);
      do {
        iVar6 = 0;
        if (*(int *)(param_1 + 0x3f4) != 0) {
          return local_54;
        }
        iVar7 = 0;
        local_50 = 0;
        if (((short)piVar5[-1] != -1) &&
           (uVar1 = *(ushort *)(param_4 + (uint)*(ushort *)(piVar5[-2] + 0x20) * 2), uVar1 != 0)) {
          iVar7 = (uint)uVar1 * 0x20 + param_5;
          local_50 = param_6 + (uint)uVar1 * 4;
        }
        param_2 = 0;
        if (((short)piVar5[1] != -1) &&
           (uVar1 = *(ushort *)(param_4 + (uint)*(ushort *)(*piVar5 + 0x20) * 2), uVar1 != 0)) {
          iVar6 = (uint)uVar1 * 0x20 + param_5;
          param_2 = param_6 + (uint)uVar1 * 4;
        }
        if ((short)piVar5[-1] == -1) {
          FUN_0055bd80(piVar5[-2],0,0,local_40);
        }
        else {
          FUN_0055abb0(piVar2,piVar5[-2],local_40);
        }
        if ((short)piVar5[1] == -1) {
          FUN_0055bd80(*piVar5,0,0,local_20);
        }
        else {
          FUN_0055abb0(piVar2,*piVar5,local_20);
        }
        *(undefined4 *)(param_1 + 0x380) = 0;
        *(undefined4 *)(param_1 + 0x398) = 0;
        *(undefined4 *)(param_1 + 0x38c) = 0;
        *(undefined4 *)(param_1 + 0x3d8) = 0;
        iVar7 = FUN_00575c60(piVar9,piVar5 + -2,iVar7,local_40,local_20,piVar5);
        if ((iVar7 != 0) &&
           (iVar6 = FUN_00575c60(piVar9,piVar5,iVar6,local_20,local_40,piVar5 + -2), iVar6 != 0)) {
          uVar4 = FUN_00576640(piVar9,iVar7,iVar6,piVar5[2],uVar3);
          uVar8 = 0;
          if (uVar4 != 0) {
            do {
              iVar6 = FUN_005729a0(param_1,*piVar9 + uVar8 * 8,local_50,param_2);
              local_54 = local_54 + iVar6;
              uVar8 = uVar8 + 1;
            } while (uVar8 < uVar4);
          }
        }
        local_4c = local_4c + 1;
        piVar5 = piVar5 + 5;
      } while (local_4c < param_3);
    }
    return local_54;
  }
  return 0;
}
