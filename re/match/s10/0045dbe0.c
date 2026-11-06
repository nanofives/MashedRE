// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x0045dbe0 (h3): 100% (regs 100%), whole symbol byte-identical. Raw converter did not compile
   (goto labels). Plain 3-case switch then clamp to [0,1]; the compiler threads case 2 (store 0.0f) past the clamp
   by itself. Float literals 0.2f @0x005cc9c0, 1.0f @0x005cc320, 0.0f @0x005d757c. Residual: none. */
/* ---- MATCH-S10 preamble (same text in every re/match/s10 source; not part of any function body) ----
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
MGLOBAL(float, DAT_0068fcbc, 0x0068fcbc);
MGLOBAL(float, DAT_006900d4, 0x006900d4);
int FUN_00493f70(int p0);
void FUN_004943f0(float p0);
// MATCH: 0x0045dbe0 FUN_0045dbe0
void FUN_0045dbe0(int param_1)
{
  switch (param_1) {
  case 0:
    DAT_0068fcbc = DAT_0068fcbc - 0.2f;
    break;
  case 1:
    DAT_0068fcbc = DAT_0068fcbc + 0.2f;
    break;
  case 2:
    DAT_0068fcbc = 0.0f;
    break;
  }
  if (DAT_0068fcbc > 1.0f) {
    DAT_0068fcbc = 1.0f;
  }
  else if (DAT_0068fcbc < 0.0f) {
    DAT_0068fcbc = 0.0f;
  }
  if (FUN_00493f70(0) != 0) {
    FUN_004943f0(DAT_006900d4 * DAT_0068fcbc);
  }
}
