// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x0047aa50: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs12_20261102/results.tsv. */
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
MGLOBAL(int, DAT_006bf1cc, 0x006bf1cc);
MGLOBAL(int, DAT_006bf1c8, 0x006bf1c8);
int FUN_004b6fc0(int * p0);
int FUN_004b70d0(undefined4 p0, undefined4 p1);
// MATCH: 0x0047aa50 FUN_0047aa50
undefined4 FUN_0047aa50(undefined4 param_1)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  char *pcVar4;
  char *pcVar5;
  
  uVar3 = FUN_004b6fc0(param_1);
  iVar2 = *(int *)(DAT_006bf1cc + 0x1d4);
  pcVar4 = (char *)FUN_004b70d0(param_1,uVar3);
  pcVar5 = (char *)((iVar2 + 0x91) * 0x40 + DAT_006bf1c8);
  do {
    cVar1 = *pcVar4;
    pcVar4 = pcVar4 + 1;
    *pcVar5 = cVar1;
    pcVar5 = pcVar5 + 1;
  } while (cVar1 != '\0');
  *(int *)(DAT_006bf1cc + 0x1d4) = *(int *)(DAT_006bf1cc + 0x1d4) + 1;
  return 0;
}
