// FLAGS: /O2 /GS /Ob1
/* MATCH-S19 0x004c3b30 (h3): 100% (regs 100%). _rwSqrt(float): RW table sqrt through a RwSplitBits union (float + volatile int/uint members,
   the param slot reused for the union) and a `const volatile unsigned int *` table pointer at
   *(RwGlobals + DAT_007d3ffc); the volatile table read keeps the lookup before the exponent reload (non-volatile
   table: 95.5%, one load hoisted). */
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
typedef union RwSplitBits { float nReal; volatile int nInt; volatile unsigned int nUInt; } RwSplitBits;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d3ffc, 0x007d3ffc);

// MATCH: 0x004c3b30 FUN_004c3b30
float FUN_004c3b30(float num)
{
  RwSplitBits s;
  s.nReal = num;
  if (s.nInt) {
    const volatile unsigned int *tab = *(volatile unsigned int **)(DAT_007d3ff8 + DAT_007d3ffc);
    s.nUInt += 0x800;
    s.nUInt = tab[(s.nUInt >> 12) & 0xfff] + ((s.nUInt >> 1) & 0x3fc00000);
  }
  return s.nReal;
}
