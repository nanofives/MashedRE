// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 h1 0x00579e50: 60.4% (regs 87.5%), class b. callee-saved reg assignment ebx/edi swapped. Tried: >=1 compare, term orders (all 12), permuter. cxx:ok. */
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
// MATCH: 0x00579e50 FUN_00579e50
void FUN_00579e50(int param_1)
{
  float *pfVar1;
  int iVar2;
  uint uVar3;
  float *pfVar4;
  uint uVar5;
  int iVar6;
  int iVar7;

  uVar5 = 1;
  uVar3 = *(uint *)(param_1 + 0x1a4) | *(uint *)(param_1 + 0x1ac);
  iVar6 = 0;
  if ((int)uVar3 >= 1) {
    iVar7 = 0x18;
    pfVar4 = (float *)(param_1 + 8);
    do {
      if ((uVar5 & uVar3) != 0) {
        iVar2 = *(int *)(param_1 + 0x1a8);
        pfVar1 = (float *)(param_1 + iVar2 * 0x18);
        *(float *)(param_1 + (iVar2 + iVar7) * 4) =
             pfVar4[-2] * *pfVar1 + pfVar4[-1] * *(float *)(param_1 + 4 + iVar2 * 0x18) + pfVar1[2] * *pfVar4;
        *(undefined4 *)(param_1 + (iVar6 + 0x18 + *(int *)(param_1 + 0x1a8) * 4) * 4) =
             *(undefined4 *)(param_1 + (iVar7 + *(int *)(param_1 + 0x1a8)) * 4);
      }
      uVar5 = uVar5 * 2;
      uVar3 = *(uint *)(param_1 + 0x1a4) | *(uint *)(param_1 + 0x1ac);
      iVar6 = iVar6 + 1;
      iVar7 = iVar7 + 4;
      pfVar4 = pfVar4 + 6;
    } while ((int)uVar5 <= (int)uVar3);
  }
}
