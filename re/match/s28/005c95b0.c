// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 0x005c95b0: 30.1% (regs 53.0%), raw Ghidra structure kept (param_1 reused as the bit counter; pointer-cast compare made C++ clean). A clean typed rewrite (unsigned char **buf, separate lo/hi) scored only 17-21%. Orig hoists (param_4!=0) into a byte stack temp and keeps all bit locals in stack bytes; structure not reproduced. Class: b. */
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
typedef unsigned char byte;
// MATCH: 0x005c95b0 FUN_005c95b0
void FUN_005c95b0(int *param_1,uint param_2,int param_3,int param_4)

{
  int *piVar1;
  byte bVar2;
  byte bVar3;
  byte bVar4;
  byte bVar5;
  uint uVar6;
  uint uVar7;
  uint local_8;
  
  piVar1 = param_1;
  bVar2 = (byte)param_2 & 7;
  uVar6 = param_2 + param_3 >> 3;
  bVar3 = (char)param_3 + (byte)param_2 & 7;
  for (uVar7 = param_2 >> 3; uVar7 <= uVar6; uVar7 = uVar7 + 1) {
    param_1 = (int *)0x0;
    local_8 = 8;
    bVar4 = 0;
    if (uVar7 == param_2 >> 3) {
      param_1 = (int *)(uint)bVar2;
      bVar4 = bVar2;
    }
    bVar5 = 8;
    if (uVar7 == uVar6) {
      local_8 = (uint)bVar3;
      bVar5 = bVar3;
    }
    if ((bVar4 == 0) && (bVar5 == 8)) {
      *(byte *)(uVar7 + *piVar1) = ~((param_4 != 0) - 1U);
    }
    else {
      for (; (uint)param_1 < local_8; param_1 = (int *)((int)param_1 + 1)) {
        *(byte *)(*piVar1 + uVar7) =
             *(byte *)(*piVar1 + uVar7) & ~('\x01' << ((byte)param_1 & 0x1f));
        *(byte *)(*piVar1 + uVar7) =
             *(byte *)(*piVar1 + uVar7) | (param_4 != 0) << ((byte)param_1 & 0x1f);
      }
    }
  }
  return;
}
