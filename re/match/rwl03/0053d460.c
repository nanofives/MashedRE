// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x0053d460: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
#ifdef __cplusplus
typedef int (*codeptr)(...);
#else
typedef int (*codeptr)();
#endif
void *memcpy(void *d, const void *s, unsigned int n);
#pragma intrinsic(memcpy)
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
undefined4 FUN_004d7ff0(undefined4 p0, int p1);
undefined4 * FUN_004d8480(undefined4 * p0);
int FUN_0053dba0(int p0, int p1, int p2, int p3, int p4);
// MATCH: 0x0053d460 FUN_0053d460
int FUN_0053d460(undefined4 param_1,undefined4 param_2,undefined4 param_3,uint param_4,
                undefined4 param_5,undefined4 *param_6,byte *param_7)

{
  undefined4 *puVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  undefined4 *puVar6;
  undefined4 *puVar7;
  undefined4 loc[3];
  
  if (param_7 == (byte *)0x0) {
    param_7 = (byte *)loc;
    loc[0] = 0;
    loc[1] = 0;
    loc[2] = 0;
  }
  if ((*param_7 & 1) != 0) {
  iVar2 = (**(codeptr *)(DAT_007d3ff8 + 0x108))(param_4 * 10,0x1011d);
  if (iVar2 != 0) {
    puVar1 = (undefined4 *)(iVar2 + param_4 * 2);
    memcpy(puVar1, param_6, param_4 * 8);
    iVar4 = FUN_0053dba0(param_2,param_3,param_4,param_5,iVar2);
    if (iVar4 != 0) {
      uVar3 = 0;
      if (param_4 != 0) {
        do {
          uVar5 = (uint)*(ushort *)(iVar2 + uVar3 * 2);
          uVar3 = uVar3 + 1;
          *param_6 = puVar1[uVar5 * 2];
          param_6[1] = puVar1[uVar5 * 2 + 1];
          param_6 = param_6 + 2;
        } while (uVar3 < param_4);
      }
      if (*(codeptr *)(param_7 + 4) != (codeptr)0) {
        (**(codeptr *)(param_7 + 4))(param_1,iVar4,iVar2,*(undefined4 *)(param_7 + 8));
      }
    }
    (**(codeptr *)(DAT_007d3ff8 + 0x10c))(iVar2);
    return iVar4;
  }
  loc[0] = 0x11d;
  loc[1] = FUN_004d7ff0(0x80000013,param_4 * 10);
  FUN_004d8480(loc);
  return 0;
  }
  iVar2 = FUN_0053dba0(param_2,param_3,param_4,param_5,0);
  return iVar2;
}
