// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWR04 2026-11-08: 100% (regs 100%), relocheck OK, C++ check OK, was 90.2%. return param_2 duplicated inside the if block, so the shared epilogue pushes edi at entry. Supersedes the header below. */
/* MATCH-RWL05 0x0055ade0 (h2): hand, 90.2%, class (a): residue is push edi hoisted to the prologue in the original (shrink-wrap), flags /GS /O1 /G6 worse, permuter 150 s no gain. */
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
typedef int (*FN)(int *);
// MATCH: 0x0055ade0 FUN_0055ade0
int * FUN_0055ade0(int param_1,int *param_2)
{
  unsigned int *puVar1;

  if ((param_2[9] != 0) &&
     ((*(unsigned int *)(*(int *)(param_2[9] + 0x60) + ((unsigned int)*(unsigned short *)(param_2 + 8) >> 5) * 4) &
      1 << ((unsigned int)*(unsigned short *)(param_2 + 8) & 0x1f)) != 0)) {
    (**(FN *)(*param_2 + 0x14))(param_2);
    puVar1 = (unsigned int *)(*(int *)(param_1 + 0x60) + ((unsigned int)*(unsigned short *)(param_2 + 8) >> 5) * 4);
    *puVar1 = *puVar1 & ~(1 << ((unsigned int)*(unsigned short *)(param_2 + 8) & 0x1f));
    *(int *)(param_1 + 0x4c) = *(int *)(param_1 + 0x4c) + -1;
    return param_2;
  }
  return param_2;
}
