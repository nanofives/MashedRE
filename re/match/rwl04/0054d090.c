// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x0054d090 h1: best 36.8% (regs 37.2%) from permuter litcmp; structure only (150 B, nested loops, 57 ins, register pressure with stack counters: orig keeps loop counters [esp+0x14]/[esp+0x28], ours different alloc). Class (b). */
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
// MATCH: 0x0054d090 FUN_0054d090
short * FUN_0054d090(short *param_1,short param_2,int param_3)
{
  int iVar1;
  int iVar2;
  short *psVar3;
  int iVar4;
  unsigned int uVar5;
  short *local_c;
  int local_8;
  
  iVar2 = param_3;
  uVar5 = 1;
  iVar1 = param_3 + 1;
  local_c = param_1;
  iVar4 = 1;
  if (param_3 >= 1) {
    local_8 = param_3;
    psVar3 = param_1;
    do {
      param_3 = iVar2 + 1;
      do {
        *psVar3 = param_2;
        psVar3[iVar4] = (short)iVar2 + 1 + param_2;
        psVar3 = psVar3 + iVar4 + iVar4;
        param_2 = param_2 + 1;
        param_3 = param_3 + -1;
      } while (param_3 != 0);
      iVar4 = -iVar4;
      local_8 = local_8 + -1;
      local_c = local_c + iVar1 * 2;
      psVar3 = local_c + ((iVar2 + 1) * 2 + iVar4) * uVar5;
      uVar5 = (unsigned int)(uVar5 == 0);
    } while (local_8 != 0);
  }
  return param_1;
}
