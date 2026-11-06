// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00496e40 (h3): by hand, 100% (regs 100%), relocheck OK. */
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
typedef void *(__cdecl *PFN_CB)(void *a, void *b);
MGLOBAL(void *, DAT_00773094, 0x00773094);
MGLOBAL(void *, DAT_00773084, 0x00773084);
MGLOBAL(void *, DAT_00773088, 0x00773088);
MGLOBAL(void *, DAT_0077308c, 0x0077308c);
void *FUN_00496d60(void);
void *FUN_00496db0(void);
void **FUN_004eb3c0(void);
void **FUN_004eb9d0(void);
void *FUN_004d4f00(void *a, void *b, int c, int d);
void *FUN_004ec750(void *p);
void FUN_004ec740(void *p, PFN_CB cb);
void *FUN_00496e00(void *a, void *b);
void *FUN_00496e20(void *a, void *b);
// MATCH: 0x00496e40 FUN_00496e40
void FUN_00496e40(void)
{
  void *p;
  DAT_00773094 = FUN_00496d60();
  p = FUN_004d4f00(DAT_00773094, *FUN_004eb3c0(), 0, 0);
  DAT_00773084 = FUN_004ec750(p);
  FUN_004ec740(p, FUN_00496e00);
  DAT_00773088 = FUN_00496db0();
  p = FUN_004d4f00(DAT_00773088, *FUN_004eb9d0(), 0, 0);
  DAT_0077308c = FUN_004ec750(p);
  FUN_004ec740(p, FUN_00496e20);
}
