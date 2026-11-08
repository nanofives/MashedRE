// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x00558df0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl05_20261107/results.tsv. */
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
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef int code();
#ifndef __cplusplus
#define true 1
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
uint FUN_004cbd30(undefined4 * p0, undefined4 * p1, uint p2);
undefined4 FUN_004cc5e0(undefined4 p0, int p1, undefined4 * p2, uint * p3);
undefined4 FUN_004d7ff0(undefined4 p0);
undefined4 * FUN_004d8480(undefined4 * p0);
undefined4 FUN_0055deb0(int p0);
undefined4 FUN_0055dec0(undefined4 * p0);
int * FUN_005c4ad0(int p0, uint p1);
int FUN_005c4bb0(int * p0, undefined4 p1);
void FUN_005c4d20(int p0, int p1);
undefined4 FUN_005c4d50(int * p0);
int FUN_005c4df0(int * p0, int p1);
int FUN_005c4e10(int * p0);
// MATCH: 0x00558df0 FUN_00558df0
int * FUN_00558df0(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  undefined4 *puVar4;
  int *piVar5;
  int *piVar6;
  undefined4 *puVar7;
  uint local_10;
  undefined4 local_c;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  iVar1 = param_2;
  iVar2 = FUN_004cc5e0(param_2,1,&local_c,&local_10);
  if (iVar2 == 0) {
    return (int *)0x0;
  }
  if (local_10 < *(uint *)(param_1 + 0xc)) {
    piVar3 = (int *)(**(code **)(param_1 + 0x2c))(iVar1);
    return piVar3;
  }
  iVar2 = FUN_004cbd30(iVar1,&param_2,local_c);
  if (iVar2 == 0) {
    return (int *)0x0;
  }
  piVar3 = (int *)(**(code **)(DAT_007d3ff8 + 0x108))(8,*(uint *)(param_1 + 4) | 0x30000);
  if (piVar3 == (int *)0x0) {
    uStack_8 = 0x1bf;
    uStack_4 = FUN_004d7ff0(0x80000013,8);
    FUN_004d8480(&uStack_8);
    return (int *)0x0;
  }
  iVar2 = FUN_005c4ad0(4,*(uint *)(param_1 + 4) | 0x30000);
  piVar3[1] = iVar2;
  if (iVar2 != 0) {
    *piVar3 = param_1;
    puVar4 = (undefined4 *)
             FUN_005c4bb0(*(undefined4 *)(param_1 + 0x10),*(uint *)(param_1 + 4) | 0x30000);
    if (puVar4 != (undefined4 *)0x0) {
      *puVar4 = piVar3;
      if (param_2 == 0) {
        return piVar3;
      }
      while( true ) {
        param_2 = param_2 + -1;
        iVar2 = FUN_004cc5e0(iVar1,*(undefined4 *)(param_1 + 8),&local_c,&local_10);
        if ((iVar2 == 0) || (iVar2 = (**(code **)(param_1 + 0x28))(iVar1), iVar2 == 0)) break;
        piVar5 = (int *)FUN_005c4bb0(piVar3[1],*(uint *)(*piVar3 + 4) | 0x30000);
        *piVar5 = iVar2;
        (**(code **)(*piVar3 + 0x18))(iVar2);
        (**(code **)(param_1 + 0x1c))(iVar2);
        if (param_2 == 0) {
          return piVar3;
        }
      }
      if (piVar3 == *(int **)(*piVar3 + 0x14)) {
        *(undefined4 *)(*piVar3 + 0x14) = 0;
      }
      iVar1 = *piVar3;
      piVar5 = (int *)FUN_0055dec0(*(undefined4 *)(iVar1 + 0x10));
      piVar6 = (int *)FUN_005c4e10(*(undefined4 *)(iVar1 + 0x10));
      do {
        if (piVar5 == piVar6) {
LAB_00558fd8:
          puVar7 = (undefined4 *)FUN_005c4e10(piVar3[1]);
          for (puVar4 = (undefined4 *)FUN_0055dec0(piVar3[1]); puVar4 != puVar7; puVar4 = puVar4 + 1
              ) {
            (**(code **)(*piVar3 + 0x1c))(*puVar4);
          }
          FUN_005c4d50(piVar3[1]);
          (**(code **)(DAT_007d3ff8 + 0x10c))(piVar3);
          return (int *)0x0;
        }
        if (piVar3 == (int *)*piVar5) {
          iVar2 = FUN_0055deb0(*(undefined4 *)(iVar1 + 0x10));
          piVar6 = (int *)FUN_005c4df0(*(undefined4 *)(iVar1 + 0x10),iVar2 + -1);
          *piVar5 = *piVar6;
          FUN_005c4d20(*(undefined4 *)(iVar1 + 0x10),1);
          goto LAB_00558fd8;
        }
        piVar5 = piVar5 + 1;
      } while( true );
    }
    FUN_005c4d50(piVar3[1]);
  }
  (**(code **)(DAT_007d3ff8 + 0x10c))(piVar3);
  return (int *)0x0;
}
