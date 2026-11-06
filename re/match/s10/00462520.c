// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x00462520 (h6): 100.0% (regs 100%). Raw 90.3% (switch rebased with dec eax, 12-entry table).
   The original's 13-entry table needs `case 0: return 0x1e;` as its own first case (the compiler merges it
   with the default body after building the table); a shared `case 0: default:` label is dropped.
   Residual: none. */
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
int FUN_00430790(void);
// MATCH: 0x00462520 FUN_00462520
int FUN_00462520(void)
{
  switch (FUN_00430790()) {
  case 0:
    return 0x1e;
  case 1:
    return 3;
  case 2:
    return 0x27;
  case 3:
    return 0;
  case 4:
    return 0x26;
  case 5:
    return 0xb;
  case 6:
    return 0x19;
  case 7:
    return 2;
  case 8:
    return 0x24;
  case 9:
    return 0x22;
  case 10:
    return 0x25;
  case 0xb:
    return 0x21;
  case 0xc:
    return 0x1a;
  default:
    return 0x1e;
  }
}
