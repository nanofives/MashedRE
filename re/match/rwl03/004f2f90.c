// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x004f2f90: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
#ifdef __cplusplus
typedef int (__cdecl code)(...);
#else
typedef int code();
#endif
#ifndef __cplusplus
#define true 1
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
// MATCH: 0x004f2f90 FUN_004f2f90
void FUN_004f2f90(undefined4 param_1,int *param_2,short param_3,short param_4,int param_5,
                 int param_6)

{
  undefined4 uVar1;
  int iVar2;
  short *psVar3;
  
  for (psVar3 = (short *)*param_2; psVar3 != (short *)0x0; psVar3 = *(short **)(psVar3 + 6)) {
    if (((param_3 == psVar3[1]) && (param_4 == *psVar3)) && (*(int *)(psVar3 + 4) == 0)) {
      *(char *)(*(int *)(psVar3 + 2) + 0x20) = *(char *)(*(int *)(psVar3 + 2) + 0x20) + '';
      iVar2 = *(int *)(param_6 + param_5 * 4);
      *(char *)(iVar2 + 0x20) = *(char *)(iVar2 + 0x20) + '';
      *(undefined4 *)(psVar3 + 4) = *(undefined4 *)(param_6 + param_5 * 4);
      return;
    }
  }
  psVar3 = (short *)(**(code **)(DAT_007d3ff8 + 0x118))(param_1,0x30502);
  *psVar3 = param_3;
  psVar3[1] = param_4;
  *(undefined4 *)(psVar3 + 2) = *(undefined4 *)(param_6 + param_5 * 4);
  *(int *)(psVar3 + 4) = 0;
  *(int *)(psVar3 + 6) = *param_2;
  *param_2 = (int)psVar3;
}
