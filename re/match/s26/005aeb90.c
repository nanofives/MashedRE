// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 h1 0x005aeb90 (98 B). Best 43.3% (regs 43.3%), residual class (c). orig expands log10 inline (fldlg2; fld; fyl2x) which cl 9176 never does for log10 (call, with or without #pragma intrinsic, /Oi, /Og, /G5-7); only reachable via __asm. */
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
double __cdecl log10(double);
#pragma intrinsic(log10)
MGLOBAL(float, DAT_005cc990, 0x005cc990);
MGLOBAL(float, DAT_005cd0b8, 0x005cd0b8);
// MATCH: 0x005aeb90 FUN_005aeb90
int FUN_005aeb90(float param_1)
{
  int v;
  if (param_1 < DAT_005cc990) {
    return -10000;
  }
  v = (int)(float)(log10(param_1) * DAT_005cd0b8);
  return v & -(v > 0 ? 0 : 1) ;
}
