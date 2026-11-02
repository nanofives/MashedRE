// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c5cb0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned int undefined4;
typedef int code();
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d4054, 0x007d4054);
extern char s_(null)_00618178[];
undefined4 FUN_004d7ff0(undefined4 p0);
undefined4 * FUN_004d8480(undefined4 * p0);
MGLOBAL(int, s__null__00618178, 0x00618178);
// MATCH: 0x004c5cb0 FUN_004c5cb0
int FUN_004c5cb0(undefined4 param_1,int param_2)

{
  undefined4 *puVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  int *piVar5;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  iVar4 = (**(code **)(DAT_007d3ff8 + DAT_007d4054 + 0x18))(param_1);
  if (iVar4 != 0) {
    *(int *)(iVar4 + 0x54) = *(int *)(iVar4 + 0x54) + 1;
    return iVar4;
  }
  iVar4 = (**(code **)(DAT_007d3ff8 + DAT_007d4054 + 0x14))(param_1,param_2);
  if (iVar4 == 0) {
    uStack_8 = 1;
    if (param_2 != 0) {
      uStack_4 = FUN_004d7ff0(0x16,param_1,param_2);
      FUN_004d8480(&uStack_8);
      return 0;
    }
    uStack_4 = FUN_004d7ff0(0x16,param_1,s__null__00618178);
    FUN_004d8480(&uStack_8);
    return 0;
  }
  iVar2 = *(int *)(DAT_007d3ff8 + DAT_007d4054 + 0x10);
  if (iVar2 != 0) {
    if (*(int *)(iVar4 + 4) != 0) {
      **(undefined4 **)(iVar4 + 0xc) = *(undefined4 *)(iVar4 + 8);
      *(undefined4 *)(*(int *)(iVar4 + 8) + 4) = *(undefined4 *)(iVar4 + 0xc);
    }
    *(int *)(iVar4 + 4) = iVar2;
    uVar3 = *(undefined4 *)(iVar2 + 8);
    piVar5 = (int *)(iVar2 + 8);
    puVar1 = (undefined4 *)(iVar4 + 8);
    *(int **)(iVar4 + 0xc) = piVar5;
    *puVar1 = uVar3;
    *(undefined4 **)(*piVar5 + 4) = puVar1;
    *piVar5 = (int)puVar1;
  }
  return iVar4;
}
