// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00553030: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef undefined4 *(*PFN_f)(undefined4,int);
MGLOBAL(undefined4, DAT_00912a24, 0x00912a24);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
int * FUN_00553f40(void * p0);
int FUN_0055deb0(int p0);
int FUN_005c4bb0(int p0, undefined4 p1);
// MATCH: 0x00553030 FUN_00553030
int FUN_00553030(int param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  undefined4 *puVar2;
  
  iVar1 = FUN_0055deb0(**(undefined4 **)(param_1 + 0x18));
  if (iVar1 == 0) {
    puVar2 = (undefined4 *)FUN_005c4bb0(**(undefined4 **)(param_1 + 0x18),0x30190);
    *puVar2 = 0;
    puVar2[1] = param_2;
    puVar2[2] = param_3;
    puVar2[3] = 0;
    puVar2[4] = 0;
    puVar2[5] = 0;
    return param_1;
  }
  puVar2 = (undefined4 *)(**(PFN_f *)(DAT_007d3ff8 + 0x118))(DAT_00912a24,0x301a1);
  if (puVar2 != (undefined4 *)0x0) {
    *puVar2 = 0;
    puVar2[1] = 0;
    puVar2[2] = 0;
    puVar2[3] = 1;
    puVar2[5] = 0;
    puVar2[6] = (undefined4)puVar2;
    puVar2[4] = 0;
    FUN_00553f40(puVar2);
  }
  *(undefined4 **)(*(int *)(param_1 + 0x18) + 0x14) = puVar2;
  FUN_00553f40((void *)*(undefined4 *)(*(int *)(param_1 + 0x18) + 0x14));
  *(undefined4 *)(param_1 + 0x18) = *(undefined4 *)(*(int *)(param_1 + 0x18) + 0x14);
  FUN_00553030(param_1,param_2,param_3);
  return param_1;
}
