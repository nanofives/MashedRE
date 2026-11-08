// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x0054cfa0 h1: best 65.8% (regs 66.2%) from permuter; structure only (225 B nested loops, stack counters; orig uses esi/edx for stride/value). Class (b). File created by hand from decomp. */
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
// MATCH: 0x0054cfa0 FUN_0054cfa0
short * FUN_0054cfa0(short *param_1,short param_2,unsigned int param_3)
{
  unsigned int uVar1;
  short *psVar2;
  short sVar3;
  int iVar4;
  int iVar5;
  unsigned int uVar6;
  int local_14;
  int local_10;
  short *local_c;
  int cnt;
  
  uVar6 = 1;
  iVar4 = 0;
  psVar2 = param_1;
  local_c = param_1;
  iVar5 = 1;
  if (0 < (int)param_3) {
    local_14 = param_3 * 4 + 2;
    local_10 = param_3 * 2 + 1;
    uVar1 = param_3;
    sVar3 = param_2;
    do {
      if (0 < (int)uVar1) {
        cnt = uVar1;
        do {
          *psVar2 = sVar3;
          psVar2[iVar5] = ((short)param_3 - (short)iVar4) + 1 + sVar3;
          psVar2 = psVar2 + iVar5 + iVar5;
          sVar3 = sVar3 + 1;
          cnt = cnt - 1;
        } while (cnt != 0);
      }
      *psVar2 = sVar3;
      sVar3 = sVar3 + 1;
      local_c = (short *)((int)local_14 + local_c);
      local_14 = local_14 + -4;
      psVar2 = local_c + ((local_10 - iVar5) + -2) * uVar6;
      uVar6 = (unsigned int)(uVar6 == 0);
      uVar1 = uVar1 - 1;
      local_10 = local_10 + -2;
      iVar5 = -iVar5;
      iVar4 = iVar4 + 1;
    } while (iVar4 < (int)param_3);
  }
  if ((param_3 & 1) != 0) {
    *psVar2 = psVar2[-1];
  }
  return param_1;
}
