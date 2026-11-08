// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 0x004e8400: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl02_20261107/results.tsv. */
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
MGLOBAL(undefined4, DAT_006186a4, 0x006186a4);
int FUN_004cf460(int p0);
int FUN_004e1ce0(int p0, undefined4 p1);
// MATCH: 0x004e8400 FUN_004e8400
int FUN_004e8400(int *param_1)

{
  int iVar1;
  int iVar2;
  
  iVar2 = 0x28;
  if (*param_1 != 0) {
    iVar2 = FUN_004cf460(*param_1);
    iVar2 = iVar2 + 0x34;
  }
  iVar1 = FUN_004e1ce0((int)&DAT_006186a4,(undefined4)param_1);
  return iVar1 + 0xc + iVar2;
}
