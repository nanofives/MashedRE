// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x00530160: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned int undefined4;
typedef unsigned int uint;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
uint FUN_004cbd30(undefined4 p0, int p1, uint p2);
undefined4 FUN_004cc790(undefined4 p0, void *p1, undefined4 p2);
// MATCH: 0x00530160 FUN_00530160
undefined4 FUN_00530160(undefined4 param_1,int *param_2)

{
  int *piVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 uVar4;
  uint uVar5;
  int local_14;
  int local_10;
  int local_c;
  uint local_8;
  int local_4;
  
  piVar1 = param_2 + 9;
  iVar2 = FUN_004cc790(param_1,&local_c,4);
  if (iVar2 == 0) {
    return 0;
  }
  iVar2 = FUN_004cc790(param_1,&local_14,4);
  if (iVar2 != 0) {
    iVar2 = FUN_004cc790(param_1,&local_10,4);
    if (iVar2 == 0) {
      return 0;
    }
    uVar4 = param_1;
    if (0 < local_14) {
      local_4 = local_c;
      iVar2 = *param_2;
      if (param_2[0xc] != 0) {
        (**(code **)(DAT_007d3ff8 + 0x10c))(param_2[0xc]);
      }
      *piVar1 = 0;
      param_2[10] = 0;
      param_2[0xb] = 0;
      param_2[0xc] = 0;
      param_2[0xd] = 0;
      param_2[0xe] = 0;
      local_8 = iVar2 + (local_14 + local_10) * 2;
      puVar3 = (undefined4 *)(**(code **)(DAT_007d3ff8 + 0x108))(local_8,0x30116);
      param_2[0xc] = (int)puVar3;
      uVar4 = 0;
      if (puVar3 != (undefined4 *)0x0) {
        for (uVar5 = local_8 >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
          *puVar3 = 0;
          puVar3 = puVar3 + 1;
        }
        for (uVar5 = local_8 & 3; uVar5 != 0; uVar5 = uVar5 - 1) {
          *(undefined1 *)puVar3 = 0;
          puVar3 = (undefined4 *)((int)puVar3 + 1);
        }
        *piVar1 = local_4;
        param_2[10] = local_14;
        iVar2 = param_2[0xc] + iVar2;
        param_2[0xd] = iVar2;
        param_2[0xb] = local_10;
        param_2[0xe] = iVar2 + local_14 * 2;
        iVar2 = FUN_004cbd30(param_1,param_2[0xc],*param_2 + (local_10 + local_14) * 2);
        uVar4 = param_1;
        if (iVar2 == 0) {
          (**(code **)(DAT_007d3ff8 + 0x10c))(piVar1);
          return 0;
        }
      }
    }
    return uVar4;
  }
  return 0;
}
