// FLAGS: /O2 /GS /Ob1
// TOOLCHAIN ddk
/* ---- MATCH-S02 preamble (same text in every re/match/s02 source; not part of any function body) ----
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

MGLOBAL(int, DAT_00636570, 0x00636570);
MGLOBAL(undefined4, DAT_00636560, 0x00636560);
MGLOBAL(int, DAT_0063656c, 0x0063656c);
MGLOBAL(undefined4, DAT_00636564, 0x00636564);
void FUN_004c0740(int p0, int p1);
undefined4 FUN_004c0c20(int p0);
undefined4 FUN_004e4d90(undefined4 p0);
undefined4 RpClumpDestroy(int p0);
undefined4 RpWorldDestroy(int p0);
undefined4 RpWorldRemoveLight(undefined4 p0, int p1);
undefined4 RwFrameRemoveChild(undefined4 p0, int p1);
// MATCH: 0x004014b0 FUN_004014b0
void FUN_004014b0(void)

{
  RpWorldRemoveLight(DAT_00636560,DAT_00636570);
  RpWorldRemoveLight(DAT_00636560,DAT_0063656c);
  if (*(int *)(DAT_00636570 + 4) != 0) {
    FUN_004c0c20(*(int *)(DAT_00636570 + 4));
  }
  FUN_004c0740(DAT_00636570,0);
  FUN_004e4d90(DAT_00636570);
  if (*(int *)(DAT_0063656c + 4) != 0) {
    FUN_004c0c20(*(int *)(DAT_0063656c + 4));
  }
  FUN_004c0740(DAT_0063656c,0);
  FUN_004e4d90(DAT_0063656c);
  RwFrameRemoveChild(DAT_00636560,DAT_00636564);
  RpWorldDestroy(DAT_00636560);
  RpClumpDestroy(DAT_00636564);
  return;
}

