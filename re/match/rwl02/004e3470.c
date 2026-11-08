// FLAGS: /O2 /Ob1 /G5
// h2 hand-matched.
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

MGLOBAL(int, DAT_007d716c, 0x007d716c);
MGLOBAL(int, DAT_007d7170, 0x007d7170);
MGLOBAL(int, DAT_007d7174, 0x007d7174);
MGLOBAL(int, DAT_007d7178, 0x007d7178);
typedef void (*Fn)(void);
void FUN_004d7ff0(void);
void FUN_004e36a0(void);
void FUN_004e3790(void);
void FUN_004e3800(void);
void FUN_004e3b30(void);
void FUN_004e3b80(void);
void FUN_004e3bd0(void);
void FUN_004e3dc0(void);
void FUN_004e3e40(void);
void FUN_004e3e70(void);
void FUN_004e3ea0(void);
void FUN_004e40d0(void);
void FUN_004e4130(void);
void FUN_004e4190(void);
void FUN_004e41b0(void);
void FUN_004e41e0(void);
void FUN_004e4200(void);
void FUN_004e4220(void);
void FUN_004e4240(void);
void FUN_004e4250(void);
void FUN_004e4270(void);
void FUN_004e4290(void);
void FUN_004e42a0(void);
void FUN_004e42d0(void);
void FUN_004e4300(void);
void FUN_004e5ef0(void);
void FUN_004e5f30(void);
void FUN_004e5f70(void);
void FUN_004e7e50(void);
void FUN_004e7e90(void);
void FUN_004e7ed0(void);
void FUN_004e98d0(void);
void FUN_004e9910(void);
void FUN_004e9950(void);
void FUN_004e9990(void);
int FUN_004c1cc0(int p0, int p1, Fn p2, Fn p3, Fn p4);
int FUN_004c2d90(int p0, int p1, Fn p2, Fn p3);
int FUN_004e4bd0(int p0, int p1, Fn p2, Fn p3, Fn p4);
int FUN_004e5cd0(int p0, int p1, Fn p2, Fn p3, Fn p4);
int FUN_004e5d00(int p0, Fn p1, Fn p2, Fn p3);
int FUN_004e7da0(int p0, Fn p1, Fn p2, Fn p3);
int FUN_004e80c0(int p0, int p1, Fn p2, Fn p3, Fn p4);
int FUN_004e80f0(int p0, Fn p1, Fn p2, Fn p3);
int FUN_004e8f50(int p0, int p1, Fn p2, Fn p3, Fn p4);
int FUN_004e8f80(int p0, Fn p1, Fn p2, Fn p3);
int FUN_004f0910(int p0, int p1, Fn p2, Fn p3, Fn p4);
int FUN_004f0940(int p0, Fn p1, Fn p2, Fn p3);
int RpAtomicRegisterPlugin(int p0, int p1, Fn p2, Fn p3, Fn p4);
int RpClumpRegisterPlugin(int p0, int p1, Fn p2, Fn p3, Fn p4);
int FUN_004d8560(void);
// MATCH: 0x004e3470 FUN_004e3470
int FUN_004e3470(void)
{
  int r;
  r = FUN_004c2d90(0xc, 0x509, FUN_004e36a0, FUN_004e3790);
  DAT_007d716c = FUN_004c1cc0(0x1c, 0x509, FUN_004e3800, FUN_004e3b80, FUN_004e3b30);
  r |= DAT_007d716c;
  DAT_007d7170 = RpAtomicRegisterPlugin(8, 0x509, FUN_004e3bd0, FUN_004e3dc0, FUN_004d7ff0);
  r |= DAT_007d7170;
  DAT_007d7174 = RpClumpRegisterPlugin(8, 0x509, FUN_004e3e40, FUN_004d7ff0, FUN_004e3e70);
  r |= DAT_007d7174;
  DAT_007d7178 = FUN_004e4bd0(8, 0x509, FUN_004e3ea0, FUN_004e4130, FUN_004e40d0);
  r |= DAT_007d7178;
  r |= FUN_004e8f50(0, 0x50e, 0, 0, 0);
  r |= FUN_004f0910(0, 0x50e, 0, 0, 0);
  r |= FUN_004e8f80(0x50e, FUN_004e41b0, FUN_004e4190, FUN_004e41e0);
  r |= FUN_004f0940(0x50e, FUN_004e42d0, FUN_004e42a0, FUN_004e4300);
  r |= FUN_004e8f50(0, 0x510, 0, 0, 0);
  r |= FUN_004f0910(0, 0x510, 0, 0, 0);
  r |= FUN_004e8f80(0x510, FUN_004e4220, FUN_004e4200, FUN_004e4240);
  r |= FUN_004f0940(0x510, FUN_004e4270, FUN_004e4250, FUN_004e4290);
  r |= RpAtomicRegisterPlugin(0, 0x1f, 0, 0, 0);
  r |= FUN_004e7da0(0x1f, FUN_004e5ef0, FUN_004e5f30, FUN_004e5f70);
  r |= FUN_004e5cd0(0, 0x1f, 0, 0, 0);
  r |= FUN_004e5d00(0x1f, FUN_004e98d0, FUN_004e5f30, FUN_004e5f70);
  r |= FUN_004f0910(0, 0x1f, 0, 0, 0);
  r |= FUN_004f0940(0x1f, FUN_004e9910, FUN_004e9950, FUN_004e9990);
  r |= FUN_004e80c0(0, 0x1f, 0, 0, 0);
  r |= FUN_004e80f0(0x1f, FUN_004e7e50, FUN_004e7e90, FUN_004e7ed0);
  if (r < 0) {
    return 0;
  }
  return FUN_004d8560() != 0;
}
