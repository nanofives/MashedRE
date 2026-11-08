// FLAGS: /O2 /GS /Ob1
/* MATCH-S27 h1: 39.8% (regs 53.0%, permuter best (150 s). Residual not classified beyond register/schedule differences. Class: b. */
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
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
undefined4 FUN_005b6a40(short p0, int * p1, int * p2);
// MATCH: 0x005b6820 FUN_005b6820
int FUN_005b6820(ushort *param_1,uint *param_2,int param_3)

{
  ushort uVar1;
  int iVar2;
  ushort *puVar3;
  uint uVar4;
  uint *puVar5;
  uint local_c;
  int local_8;
  int local_4;
  
  puVar3 = param_1;
  local_c = param_3 + 0x3fU >> 6;
  param_1 = (ushort *)0x0;
  local_4 = local_c * 0x24;
  puVar5 = param_2;
  if (local_c == 0) {
    return 0;
  }
  do {
    uVar1 = *puVar3;
    param_3 = param_3 + -1;
    puVar3 = puVar3 + 1;
    *puVar5 = (int)param_1 << 0x10 | (uint)uVar1;
    local_8 = (int)(short)uVar1;
    puVar5 = puVar5 + 1;
    uVar4 = 0;
    param_2 = (uint *)0x0;
    do {
      if ((param_3 < 1) || (0x3e < (int)param_2)) {
        iVar2 = 0;
      }
      else {
        iVar2 = (int)(short)*puVar3;
        puVar3 = puVar3 + 1;
        param_3 = param_3 + -1;
      }
      iVar2 = FUN_005b6a40(iVar2,&local_8,(int *)&param_1);
      uVar4 = uVar4 >> 4 | iVar2 << 0x1c;
      if (((byte)param_2 & 7) == 7) {
        *puVar5 = uVar4;
        puVar5 = puVar5 + 1;
        uVar4 = 0;
      }
      param_2 = (uint *)((int)param_2 + 1);
    } while ((int)param_2 < 0x40);
    local_c = local_c - 1;
  } while (local_c != 0);
  return local_4;
}
