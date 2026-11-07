// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 h1: 20.9% (regs 35.1%), permuter best (240 s). Structure not found (large, budget spent). Class: b. */
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
typedef unsigned int uint;
// MATCH: 0x005bb5b0 FUN_005bb5b0
void FUN_005bb5b0(int param_1,int param_2,int param_3,uint param_4)

{
  int iVar1;
  short *psVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  short *psVar7;
  uint local_24;
  
  param_4 = param_4 >> 1;
  uVar4 = 0;
  local_24 = 0;
  if ((param_4 & 0xfffffffc) != 0) {
    psVar2 = (short *)(param_3 + 2);
    psVar7 = (short *)(param_2 + 6);
    do {
      iVar1 = (int)psVar2[-1] + (int)psVar7[-3];
      iVar3 = (int)*(short *)((param_2 - param_3) + (int)psVar2) + (int)*psVar2;
      iVar6 = (int)psVar2[1] + (int)psVar7[-1];
      iVar5 = (int)psVar2[2] + (int)*psVar7;
      if (iVar1 < 0x8000) {
        if (iVar1 < -0x7fff) {
          iVar1 = -0x7fff;
        }
      }
      else {
        iVar1 = 0x7fff;
      }
      *(short *)(param_1 + local_24 * 2) = (short)iVar1;
      if (!(iVar3 < 0x8000)) {
        iVar3 = 0x7fff;
      } else {
        if (iVar3 < -0x7fff) {
          iVar3 = -0x7fff;
        }
      }
      *(short *)((param_1 - param_3) + (int)psVar2) = (short)iVar3;
      if (iVar6 < 0x8000) {
        if (iVar6 < -0x7fff) {
          iVar6 = -0x7fff;
        }
      }
      else {
        iVar6 = 0x7fff;
      }
      *(short *)(param_1 + 4 + local_24 * 2) = (short)iVar6;
      if (!(iVar5 < 0x8000)) {
        iVar5 = 0x7fff;
      } else {
        if (iVar5 < -0x7fff) {
          iVar5 = -0x7fff;
        }
      }
      uVar4 = local_24 + 4;
      psVar2 = psVar2 + 4;
      *(short *)((param_1 - param_2) + (int)psVar7) = (short)iVar5;
      local_24 = uVar4;
      psVar7 = psVar7 + 4;
    } while (uVar4 < (param_4 & 0xfffffffc));
  }
  if (uVar4 < param_4) {
    psVar2 = (short *)(param_3 + uVar4 * 2);
    iVar1 = param_4 - uVar4;
    do {
      iVar5 = (int)*(short *)((int)psVar2 + (param_2 - param_3)) + (int)*psVar2;
      if (!(iVar5 < 0x8000)) {
        iVar5 = 0x7fff;
      } else {
        if (iVar5 < -0x7fff) {
          iVar5 = -0x7fff;
        }
      }
      *(short *)((int)psVar2 + (param_1 - param_3)) = (short)iVar5;
      psVar2 = psVar2 + 1;
      iVar1 = iVar1 + -1;
    } while (iVar1 != 0);
  }
  return;
}
