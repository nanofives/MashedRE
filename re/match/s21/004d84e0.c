// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d84e0 (h1, by hand): 100.0% exact, regs 100.0%. Plugin-global block at DAT_007d3ff8 + DAT_007d6c5c
   copied out as one 8-byte struct, then reset to {0, 0x80000000}. Moves: struct copy (62.5 -> 80.0), and the function
   returns its argument (eax stays live, which picks the store registers; 80.0 -> 100). relocheck OK, C++ ok. */
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
typedef struct { int a; int b; } S84;
MGLOBAL(int, DAT_007d6c5c, 0x007d6c5c);
MGLOBAL(char *, DAT_007d3ff8, 0x007d3ff8);
// MATCH: 0x004d84e0 FUN_004d84e0
S84 *FUN_004d84e0(S84 *param_1)

{
  *param_1 = *(S84 *)(DAT_007d3ff8 + DAT_007d6c5c);
  ((S84 *)(DAT_007d3ff8 + DAT_007d6c5c))->a = 0;
  ((S84 *)(DAT_007d3ff8 + DAT_007d6c5c))->b = 0x80000000;
  return param_1;
}
