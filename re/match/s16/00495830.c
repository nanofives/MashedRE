// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00495830 (h2): 100% (regs 100%). Hand-written: separate early-return ifs, intrinsic strcpy from the 0x448-stride name table. */
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
char *strcpy(char *dst, const char *src);
#pragma intrinsic(strcpy)
MGLOBAL(int, DAT_00772fac, 0x00772fac);
MGLOBALA(char, DAT_00771eb4, 0x448, 0x00771eb4);
// MATCH: 0x00495830 FUN_00495830
int FUN_00495830(int param_1, char *param_2)
{
  if (param_1 >= DAT_00772fac) {
    return 0;
  }
  if (param_2 == 0) {
    return 0;
  }
  strcpy(param_2, DAT_00771eb4 + param_1 * 0x448);
  return 1;
}
