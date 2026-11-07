// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d6ce0 (h4): 100% (regs 100%). Raw form with: the device vtable calls (+0x104, +0x124 on DAT_007d4110) typed __stdcall, the texture-pointer select as a ternary with the nonzero arm first, and the param_1 != 0 branch before the SetTexture(stage, 0) branch. Residual: none. */
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
typedef long (__stdcall *SetTexFn)(void *dev, unsigned int stage, undefined4 tex);
typedef long (__stdcall *Dev124Fn)(void *dev, undefined4 v);
MGLOBAL(int, DAT_00911ae4, 0x00911ae4);
MGLOBAL(int, DAT_007d6b14, 0x007d6b14);
MGLOBAL(int, DAT_007d6b10, 0x007d6b10);
MGLOBAL(int, DAT_007d58d4, 0x007d58d4);
MGLOBAL(undefined4, DAT_007d58d0, 0x007d58d0);
MGLOBAL(int, DAT_007d6c14, 0x007d6c14);
MGLOBAL(int, DAT_007d6bfc, 0x007d6bfc);
MGLOBAL(int, DAT_007d5870, 0x007d5870);
MGLOBAL(int, DAT_007d5874, 0x007d5874);
MGLOBAL(int *, DAT_007d4110, 0x007d4110);
MGLOBAL(undefined4, DAT_007d5168, 0x007d5168);
MGLOBAL(undefined4, DAT_007d6b30, 0x007d6b30);
// MATCH: 0x004d6ce0 FUN_004d6ce0
undefined4 FUN_004d6ce0(int param_1,int param_2)

{
  undefined4 *puVar1;
  
  puVar1 = param_1 ? (undefined4 *)(DAT_00911ae4 + param_1) : (undefined4 *)0;
  if (param_2 != 0) goto LAB_004d6dfb;
  if ((param_1 == 0) || (*(char *)(puVar1 + 2) == '\0')) {
    if ((DAT_007d6b14 == 0) || (DAT_007d6b14 = 0, DAT_007d6b10 != 0)) goto LAB_004d6dfb;
    DAT_007d58d0 = 0;
    if (DAT_007d58d4 == 0) {
      DAT_007d58d4 = 1;
      (&DAT_007d5168)[DAT_007d6c14] = 0x1b;
      DAT_007d6c14 = DAT_007d6c14 + 1;
    }
    if (DAT_007d5870 == 0) goto LAB_004d6dfb;
    DAT_007d5870 = 0;
  }
  else {
    if ((DAT_007d6b14 != 0) || (DAT_007d6b14 = 1, DAT_007d6b10 != 0)) goto LAB_004d6dfb;
    DAT_007d58d0 = 1;
    if (DAT_007d58d4 == 0) {
      DAT_007d58d4 = 1;
      (&DAT_007d5168)[DAT_007d6c14] = 0x1b;
      DAT_007d6c14 = DAT_007d6c14 + 1;
    }
    if (DAT_007d5870 == DAT_007d6bfc) goto LAB_004d6dfb;
    DAT_007d5870 = DAT_007d6bfc;
  }
  if (DAT_007d5874 == 0) {
    DAT_007d5874 = 1;
    (&DAT_007d5168)[DAT_007d6c14] = 0xf;
    DAT_007d6c14 = DAT_007d6c14 + 1;
  }
LAB_004d6dfb:
  if (param_1 != (&DAT_007d6b30)[param_2 * 6]) {
    (&DAT_007d6b30)[param_2 * 6] = param_1;
    if (param_1 != 0) {
      (*(SetTexFn *)(*DAT_007d4110 + 0x104))(DAT_007d4110,param_2,*puVar1);
      if (puVar1[1] != 0) {
        (*(Dev124Fn *)(*DAT_007d4110 + 0x124))(DAT_007d4110,*(undefined4 *)(puVar1[1] + 0x400));
        return 1;
      }
    }
    else {
      (*(SetTexFn *)(*DAT_007d4110 + 0x104))(DAT_007d4110,param_2,0);
    }
  }
  return 1;
}
