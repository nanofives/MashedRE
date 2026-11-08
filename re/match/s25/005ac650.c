// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005ac650: hand-matched (h1). */
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

MGLOBALA(undefined, DAT_005e6424, 1, 0x005e6424);
MGLOBALA(undefined, DAT_005e6414, 1, 0x005e6414);
MGLOBALA(undefined, DAT_005e6434, 1, 0x005e6434);
MGLOBALA(undefined, DAT_005e6444, 1, 0x005e6444);
MGLOBALA(undefined, DAT_005e6454, 1, 0x005e6454);
MGLOBALA(undefined, DAT_005e6474, 1, 0x005e6474);
MGLOBALA(undefined, DAT_005e64b4, 1, 0x005e64b4);
int FUN_005adf30(byte *p0, byte *p1);
// MATCH: 0x005ac650 FUN_005ac650
uint FUN_005ac650(uint *param_1)
{
  int iVar1;
  uint uVar2;
  iVar1 = FUN_005adf30((byte *)param_1[1],(byte *)&DAT_005e6424);
  if (iVar1 == 0) {
    return (uint)(byte)((byte)param_1[3] >> 3);
  }
  iVar1 = FUN_005adf30((byte *)param_1[1],(byte *)&DAT_005e6414);
  if (iVar1 == 0) {
    return 0x10;
  }
  iVar1 = FUN_005adf30((byte *)param_1[1],(byte *)&DAT_005e6434);
  if (iVar1 == 0) {
    return 4;
  }
  iVar1 = FUN_005adf30((byte *)param_1[1],(byte *)&DAT_005e6444);
  if (iVar1 == 0) {
    return 8;
  }
  iVar1 = FUN_005adf30((byte *)param_1[1],(byte *)&DAT_005e6454);
  if (iVar1 != 0) {
    iVar1 = FUN_005adf30((byte *)param_1[1],(byte *)&DAT_005e64b4);
    if (iVar1 != 0) {
      iVar1 = FUN_005adf30((byte *)param_1[1],(byte *)&DAT_005e6474);
      if (iVar1 == 0) {
        if ((int *)param_1[4] != (int *)0x0) {
          iVar1 = *(int *)param_1[4];
        }
        else {
          iVar1 = 0x1f400;
        }
        uVar2 = *param_1;
        if (uVar2 == 0) {
          uVar2 = 48000;
        }
        return (uint)(iVar1 * 0x90) / uVar2;
      }
      return 0;
    }
  }
  return (((byte)(*(byte *)((int)param_1 + 0xd) - 1) != 0) ? 0xffffffe0 : 0) + 0x24;
}
