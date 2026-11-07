// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 0x005be8c0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs28_20261107/results.tsv. */
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

MGLOBAL(undefined *, PTR_FUN_006358e0, 0x006358e0);
MGLOBAL(undefined1, DAT_007de094, 0x007de094);
uint FUN_005ae800(int * p0, undefined4 p1);
void FUN_005be930(int p0);
undefined4 * FUN_005bf610(undefined4 * p0, int p1);
// MATCH: 0x005be8c0 FUN_005be8c0
undefined4 * FUN_005be8c0(int *param_1,int param_2,int param_3)

{
  undefined4 *puVar1;
  uint uVar2;
  
  uVar2 = 0;
  if (param_3 == 0) {
    param_3 = 0x14;
  }
  if ((param_2 == 0) && (param_3 == 0x14)) {
    param_2 = (int)FUN_005ae800((int *)&DAT_007de094,0x30800);
    uVar2 = 0x80000001;
  }
  puVar1 = (undefined4 *)FUN_005bf610((undefined4 *)param_2,param_3);
  if (puVar1 != (undefined4 *)0x0) {
    FUN_005be930((int)puVar1);
    *puVar1 = (undefined4)&PTR_FUN_006358e0;
    puVar1[3] = (undefined4)param_1;
    puVar1[2] = puVar1[2] | uVar2;
    (**(void (__stdcall **)(int *))(*param_1 + 4))(param_1);
  }
  return puVar1;
}
