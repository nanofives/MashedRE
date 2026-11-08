// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h1 0x004e2030: best 100.0% (regs 100.0%), residual class -. struct-of-dwords view of the slot (5 dword stores instead of 20 byte stores). */
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

MGLOBAL(undefined4, DAT_007d4710, 0x007d4710);
void FUN_004e12b0(undefined4 p0, int p1);
// MATCH: 0x004e2030 FUN_004e2030
void FUN_004e2030(undefined2 *param_1)
{
  undefined4 *pbVar1;
  uint uVar2;
  pbVar1 = (undefined4 *)(param_1 + 10);
  *(undefined4 *)(*(int *)(param_1 + 2) + 0x10) = 1;
  uVar2 = (uint)(ushort)param_1[1];
  do {
    if (pbVar1[5] != 0) {
      if ((pbVar1[1] != 0) && ((*(byte *)pbVar1 & 2) == 0)) {
        FUN_004e12b0(DAT_007d4710,pbVar1[1]);
      }
      pbVar1[0] = 0;
      pbVar1[1] = 0;
      pbVar1[3] = 0;
      pbVar1[4] = 0;
      pbVar1[5] = 0;
    }
    pbVar1 = pbVar1 + 7;
    uVar2 = uVar2 - 1;
  } while (uVar2 != 0);
  *param_1 = 0;
}
