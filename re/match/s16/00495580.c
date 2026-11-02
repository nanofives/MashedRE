// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00495580 (h2): 100% (regs 100%). Hand-written: COM Release through a __stdcall function-pointer type. */
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
typedef void (__stdcall *ReleaseFn)(int *self);
MGLOBAL(int *, DAT_00771e78, 0x00771e78);
void FUN_004987b0(char *p0);
// MATCH: 0x00495580 FUN_00495580
void FUN_00495580(void)
{
  int *p = DAT_00771e78;
  if (p != 0) {
    FUN_004987b0("Releasing object\n");
    (*(ReleaseFn *)(*p + 8))(p);
  }
  DAT_00771e78 = 0;
}
