// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005a5d90: hand-matched (h1). */
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

typedef int (__cdecl code)(int);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
void FUN_0055b930(int p0);
undefined4 FUN_00562560(int p0);
void FUN_00564190(int p0);
// MATCH: 0x005a5d90 FUN_005a5d90
undefined4 FUN_005a5d90(int *param_1)
{
  int iVar1;
  uint uVar2;
  iVar1 = *param_1 - 1;
  *param_1 = iVar1;
  if (iVar1 == 0) {
    uVar2 = 0;
    if (*(ushort *)(param_1 + 1) > 0) {
      iVar1 = 0;
      do {
        FUN_0055b930(param_1[5] + iVar1);
        uVar2 = uVar2 + 1;
        iVar1 = iVar1 + 0x4c;
      } while (uVar2 < *(ushort *)(param_1 + 1));
    }
    FUN_00564190(param_1[4]);
    (**(code **)(DAT_007d3ff8 + 0x10c))(param_1[5]);
    (**(code **)(DAT_007d3ff8 + 0x10c))(param_1[6]);
    (**(code **)(DAT_007d3ff8 + 0x10c))(param_1[7]);
    (**(code **)(DAT_007d3ff8 + 0x10c))(param_1[10]);
    (**(code **)(DAT_007d3ff8 + 0x10c))(param_1[9]);
    FUN_00562560(param_1[3]);
    if (param_1[0xb] != 0) {
      (**(code **)(DAT_007d3ff8 + 0x10c))(param_1[0xb]);
    }
  }
  return 1;
}
