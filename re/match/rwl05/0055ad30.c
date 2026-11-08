// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWR04 2026-11-08: 100% (regs 100%), relocheck OK, C++ check OK, was 87.7%. ret = param_2 assigned before the bit set (ebx stops being the zero constant). Supersedes the header below. */
/* MATCH-RWL05 0x0055ad30: hand-matched (h3). 87.7% (regs 87.7, ops 93.8). result kept in a local that the original holds in ebx; residue is ebx also used as the zero constant for the DAT_007dc8c0 stores and compare. class (b). */
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
typedef int (*VCall)(int *);
MGLOBAL(int, DAT_007dc8c0, 0x007dc8c0);
// MATCH: 0x0055ad30 FUN_0055ad30
int *FUN_0055ad30(int param_1, int *param_2)
{
  unsigned int *puVar1;
  int *ret = 0;
  int iVar2;

  if (param_2[9] != 0 &&
      (*(unsigned int *)(*(int *)(param_2[9] + 0x60) + (unsigned int)(*(unsigned short *)(param_2 + 8) >> 5) * 4) &
       (1 << (*(unsigned short *)(param_2 + 8) & 0x1f))) != 0) {
    return param_2;
  }
  if (*(unsigned int *)(param_1 + 0x4c) < *(unsigned int *)(param_1 + 0x48)) {
    iVar2 = (*(VCall *)(*param_2 + 0x10))(param_2);
    if (iVar2 != 0) {
      ret = param_2;
      puVar1 = (unsigned int *)(*(int *)(param_1 + 0x60) + (unsigned int)(*(unsigned short *)(param_2 + 8) >> 5) * 4);
      *puVar1 = *puVar1 | (1 << (*(unsigned short *)(param_2 + 8) & 0x1f));
      DAT_007dc8c0 = 0;
      *(int *)(param_1 + 0x4c) = *(int *)(param_1 + 0x4c) + 1;
    } else if (DAT_007dc8c0 == 0) {
      DAT_007dc8c0 = 1;
    }
  }
  return ret;
}
