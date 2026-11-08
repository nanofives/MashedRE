// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00551330: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned int undefined4;
typedef undefined4 (*PFN_cpy)(undefined4,undefined4);
typedef int (*PFN_slot)(int,int);
typedef undefined4 (*PFN_alloc)(int,int);
typedef int (*PFN_len)(undefined4);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
int FUN_00550400(void);
int FUN_00550520(undefined4 p0);
int FUN_00550750(undefined4 p0);
// MATCH: 0x00551330 FUN_00551330
undefined4 FUN_00551330(int param_1,int param_2,undefined4 param_3,undefined4 param_4)
{
  PFN_alloc *puVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  if (FUN_00550400() != 0) {
    iVar2 = FUN_00550520(param_3);
    if (iVar2 != 0) {
      return 6;
    }
    (**(PFN_cpy *)(DAT_007d3ff8 + 0xcc))(param_1 + 0x50,param_3);
    iVar2 = 0;
    *(int *)(param_1 + 4) = param_2;
    *(undefined4 *)(param_1 + 0x10) = 0;
    if (iVar2 < param_2) {
      do {
        iVar3 = (**(PFN_slot *)(param_1 + 0x14))(param_1,iVar2);
        *(undefined4 *)(iVar3 + 0x20) = 1;
        iVar2 = iVar2 + 1;
        *(undefined4 *)(iVar3 + 0x18) = *(undefined4 *)(param_1 + 0x10);
      } while (iVar2 < param_2);
    }
    iVar2 = FUN_00550750(param_4);
    if (iVar2 == 0) {
      puVar1 = (PFN_alloc *)(DAT_007d3ff8 + 0x108);
      uVar4 = (*puVar1)((**(PFN_len *)(DAT_007d3ff8 + 0xf4))(param_4) + 1,0x401be);
      *(undefined4 *)(param_1 + 0xc) = uVar4;
      (**(PFN_cpy *)(DAT_007d3ff8 + 0xcc))(uVar4,param_4);
      *(undefined4 *)(param_1 + 8) = 2;
      *(undefined4 *)(param_1 + 0x20) = 0;
      *(undefined4 *)(param_1 + 0x24) = 0;
      return 1;
    }
    return 7;
  }
  return 2;
}
