// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00552fa0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef void (*PFN_d)(int);
typedef void (*PFN_e)(int,int *);
MGLOBAL(undefined4, DAT_00912a24, 0x00912a24);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
int FUN_00553e80(int p0);
void FUN_005c4ba0(int p0);
undefined4 FUN_005c4d50(int p0);
// MATCH: 0x00552fa0 FUN_00552fa0
undefined4 * FUN_00552fa0(undefined4 *param_1)

{
  int *piVar1;
  int iVar2;
  
  piVar1 = (int *)param_1[5];
  if (piVar1 != (int *)0x0) {
    iVar2 = piVar1[5];
    if (*piVar1 != 0) {
      if (piVar1[1] == -1) {
        FUN_005c4d50(*piVar1);
      }
      else {
        (**(PFN_d *)(DAT_007d3ff8 + 0x10c))(*piVar1);
      }
    }
    *piVar1 = 0;
    (**(PFN_e *)(DAT_007d3ff8 + 0x11c))(DAT_00912a24,piVar1);
    FUN_00553e80(iVar2);
  }
  param_1[5] = 0;
  FUN_005c4ba0(*param_1);
  param_1[2] = 0;
  param_1[3] = 1;
  param_1[6] = (undefined4)param_1;
  return param_1;
}
