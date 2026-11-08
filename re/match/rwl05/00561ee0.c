// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x00561ee0 (h2): hand, 100%: flags /O2 /Ob1 /G5 (not /GS), function-pointer typed callees. */
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
MGLOBAL(int, DAT_00623fac, 0x00623fac);
void FUN_00561f40(void);
void FUN_00561f50(void);
void FUN_00561f80(void);
void FUN_00561ff0(void);
void FUN_004d7ff0(void);
int FUN_004f0910(int p0, int p1, void (*p2)(void), void (*p3)(void), void (*p4)(void));
void FUN_004f0940(int p0, void (*p1)(void), void (*p2)(void), void (*p3)(void));
// MATCH: 0x00561ee0 FUN_00561ee0
int FUN_00561ee0(void)
{
  if (DAT_00623fac != -1) {
    return 0;
  }
  DAT_00623fac = FUN_004f0910(4,0x902,FUN_00561f40,FUN_004d7ff0,FUN_004d7ff0);
  FUN_004f0940(0x902,FUN_00561f80,FUN_00561f50,FUN_00561ff0);
  return DAT_00623fac != -1;
}
