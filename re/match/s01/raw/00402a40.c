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

MGLOBAL(int, DAT_00636ac8, 0x00636ac8);
MGLOBAL(undefined4, DAT_007f0a30, 0x007f0a30);
MGLOBAL(undefined4, DAT_007f09e4, 0x007f09e4);
MGLOBAL(undefined4, DAT_0067f18c, 0x0067f18c);
MGLOBAL(undefined4, DAT_0067f190, 0x0067f190);
void FUN_004014b0(void);
void FUN_004015a0(void);
void FUN_00403660(void);
void FUN_0040bd00(void);
void FUN_0040cf80(void);
void FUN_0040cfd0(void);
undefined4 FUN_004114c0(void);
void FUN_0041a3d0(void);
void FUN_0041b660(void);
void FUN_0041c0e0(void);
void FUN_0041c2c0(void);
void FUN_0041ccf0(void);
void FUN_0041d890(void);
void FUN_0041da80(void);
void FUN_0041de70(void);
void FUN_0041e0d0(void);
void FUN_0041ffb0(void);
void FUN_00425ed0(void);
undefined4 FUN_00426ba0(void);
undefined4 FUN_00426c00(void);
void FUN_00427620(void);
void FUN_00428400(void);
void FUN_0042c2a0(void);
void FUN_0045b350(void);
void FUN_0045b930(void);
void FUN_00467010(void);
void FUN_00467020(int p0);
void FUN_00467070(void);
undefined4 FUN_004671a0(int p0);
undefined4 FUN_00484130(int p0);
void FUN_00489250(void);
void FUN_00494bc0(void);
void FUN_00496ce0(void);
void FUN_004b4880(int p0);
undefined4 FUN_004c5930(int p0);
undefined4 FUN_004d8560(void);
undefined4 FUN_005581f0(int p0);
void thunk_FUN_004196f0(void);
void thunk_FUN_004210f0(void);
undefined4 thunk_FUN_00493f70(int p0);   /* PORT-TOP-04C: the body passes 0; (int) so the C++ port accepts the call, same push under C */
void thunk_FUN_00494460(int p0);   /* PORT-TOP-04C: idem */
undefined4 thunk_FUN_00496970(void);
void thunk_FUN_004b6700(void);
// MATCH: 0x00402a40 FUN_00402a40
void FUN_00402a40(void)

{
  int iVar1;
  undefined4 uVar2;
  
  FUN_00403660();
  iVar1 = thunk_FUN_00493f70(0);
  if (iVar1 != 0) {
    thunk_FUN_00494460(0);
  }
  FUN_00494bc0();
  if (DAT_00636ac8 != 0) {
    FUN_004c5930(DAT_00636ac8);
    DAT_00636ac8 = 0;
  }
  FUN_00496ce0();
  FUN_0040cf80();
  iVar1 = FUN_00426ba0();
  if (iVar1 != 0) {
    FUN_00426c00();
    FUN_0040cfd0();
  }
  FUN_004114c0();
  FUN_0041a3d0();
  FUN_0041de70();
  FUN_0041c2c0();
  FUN_0041da80();
  FUN_0041e0d0();
  FUN_0041d890();
  FUN_0041ccf0();
  FUN_0041c0e0();
  FUN_0041b660();
  FUN_00489250();
  FUN_0045b930();
  FUN_0041ffb0();
  thunk_FUN_004196f0();
  thunk_FUN_004210f0();
  FUN_004014b0();
  FUN_0040bd00();
  FUN_00484130(DAT_007f0a30);
  FUN_0045b350();
  FUN_005581f0(DAT_007f09e4);
  uVar2 = FUN_004671a0(0xffffffff);
  FUN_00467020(uVar2);
  FUN_00467070();
  FUN_004b4880(DAT_0067f18c);
  FUN_004b4880(DAT_0067f190);
  FUN_0042c2a0();
  FUN_00427620();
  thunk_FUN_00496970();
  FUN_00425ed0();
  FUN_004015a0();
  thunk_FUN_004b6700();
  FUN_00467010();
  FUN_004d8560();
  FUN_00428400();
  return;
}

