// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 h1: 54.4% (regs 68.9%, permuter best (150 s). Residual not classified beyond register/schedule differences. Class: b. */
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
#ifndef __cplusplus
#define true 1
#endif
void FUN_005b6b00(uint p0, int * p1, int * p2);
// MATCH: 0x005b6910 FUN_005b6910
undefined4 FUN_005b6910(uint *param_1,short *param_2,int param_3)

{
  uint *puVar1;
  short sVar2;
  int iVar3;
  short sVar4;
  uint uVar5;
  uint uVar6;
  short *psVar7;
  uint *puVar8;
  int local_c;
  uint local_8;
  uint local_4;
  
  psVar7 = param_2;
  puVar8 = param_1;
  while( true ) {
    if (param_3 == 0) {
      return 1;
    }
    puVar1 = puVar8 + 1;
    sVar2 = (short)*puVar8;
    param_3 = param_3 + -1;
    param_1 = (uint *)(int)sVar2;
    sVar4 = (short)*puVar1;
    local_8 = *puVar8 >> 0x10 & 0x7f;
    param_2 = (short *)(int)sVar4;
    puVar8 = puVar8 + 2;
    local_4 = *puVar1 >> 0x10 & 0x7f;
    if ((0x58 < local_8) || (0x58 < local_4)) break;
    *psVar7 = sVar2;
    psVar7[1] = sVar4;
    psVar7 = psVar7 + 2;
    local_c = 0;
    do {
      uVar5 = *puVar8;
      uVar6 = puVar8[1];
      puVar8 = puVar8 + 2;
      for (iVar3 = (local_c != 7) + 7; iVar3 != 0; iVar3 = iVar3 + -1) {
        FUN_005b6b00(uVar5 & 0xf,(int *)&param_1,(int *)&local_8);
        FUN_005b6b00(uVar6 & 0xf,(int *)&param_2,(int *)&local_4);
        *psVar7 = (short)param_1;
        uVar5 = uVar5 >> 4;
        psVar7[1] = (short)param_2;
        psVar7 = psVar7 + 2;
        uVar6 = uVar6 >> 4;
      }
      local_c = local_c + 1;
    } while (local_c < 8);
  }
  return 0;
}
