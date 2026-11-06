// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x0045c640 (h2): 100% (regs 100%). Raw converter if-chain was 52.6%; a switch on param_1
   (cases 1,2,3, default) gives the dec/je chain. */
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
typedef unsigned char undefined;
typedef unsigned char undefined1;
MGLOBAL(undefined *, DAT_0088fbc4, 0x0088fbc4);
MGLOBAL(undefined *, DAT_0088fbc8, 0x0088fbc8);
MGLOBAL(undefined1, DAT_0088e67c, 0x0088e67c);
MGLOBAL(undefined1, DAT_0088e680, 0x0088e680);
MGLOBAL(undefined1, DAT_0088eb80, 0x0088eb80);
MGLOBAL(undefined1, DAT_0088eba0, 0x0088eba0);
MGLOBAL(undefined1, DAT_0088f0b0, 0x0088f0b0);
MGLOBAL(undefined1, DAT_0088f0e0, 0x0088f0e0);
MGLOBAL(undefined1, DAT_0088f6b0, 0x0088f6b0);
MGLOBAL(undefined1, DAT_0088f6c0, 0x0088f6c0);
// MATCH: 0x0045c640 FUN_0045c640
void FUN_0045c640(int param_1)
{
  switch (param_1) {
  case 1:
    DAT_0088fbc4 = &DAT_0088e67c;
    DAT_0088fbc8 = &DAT_0088e680;
    return;
  case 2:
    DAT_0088fbc4 = &DAT_0088f0b0;
    DAT_0088fbc8 = &DAT_0088f0e0;
    return;
  case 3:
    DAT_0088fbc4 = &DAT_0088eb80;
    DAT_0088fbc8 = &DAT_0088f6c0;
    return;
  default:
    DAT_0088fbc4 = &DAT_0088f6b0;
    DAT_0088fbc8 = &DAT_0088eba0;
    return;
  }
}
