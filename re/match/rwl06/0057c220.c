// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x0057c220: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned int undefined4;
MGLOBAL(int, DAT_007dc8d8, 0x007dc8d8);
void FUN_0055dff0(int p0);
// MATCH: 0x0057c220 FUN_0057c220

int FUN_0057c220(int param_1,int param_2)

{
  int *piVar1;
  int iVar2;
  
  piVar1 = (int *)(DAT_007dc8d8 + param_1);
  iVar2 = *(int *)(DAT_007dc8d8 + param_1);
  if (iVar2 != 0) {
    *(undefined4 *)(*(int *)(iVar2 + 0x10) + 8) = 0;
    *(undefined4 *)(iVar2 + 0x30) = 0;
  }
  if (param_2 != 0) {
    *(int *)(*(int *)(param_2 + 0x10) + 8) = *(int *)(param_1 + 4) + 0x10;
    FUN_0055dff0(param_2,0);
    *(int *)(param_2 + 0x30) = param_1;
  }
  *piVar1 = param_2;
  return param_1;
}
