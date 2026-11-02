// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 h4 0x0042ee00: 100% (regs 100%), whole 60 B symbol BYTE-IDENTICAL. Raw converter dropped the string
   arguments (60.0%). By hand: param_1 0/1/2 -> tail call FUN_0040bb50("lock"/"dot"/"check"), else 0. Residual: none. */
/* ---- MATCH-S07 preamble (same text in every re/match/s07 source; not part of any function body) ----
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
MGLOBALA(char, s_lock_005cd7b8, 5, 0x005cd7b8);
MGLOBALA(char, s_dot_005cd7b4, 4, 0x005cd7b4);
MGLOBALA(char, s_check_005cd7ac, 6, 0x005cd7ac);
void *FUN_0040bb50(char *a1);

// MATCH: 0x0042ee00 FUN_0042ee00
void *FUN_0042ee00(int param_1)
{
  if (param_1 == 0)
    return FUN_0040bb50(s_lock_005cd7b8);
  if (param_1 == 1)
    return FUN_0040bb50(s_dot_005cd7b4);
  if (param_1 == 2)
    return FUN_0040bb50(s_check_005cd7ac);
  return 0;
}
