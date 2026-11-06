// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x0047a7d0: 100% (regs 100%), whole obj symbol BYTE-IDENTICAL over 31 B (relocations masked) by hand (h6).
   No row in re/console/pc_funcs.csv (match.py: "no Ghidra size"); scored with an explicit size of 31 B.
   Fix: read the counter DAT_006bf1dc into a local, store through it, write back local + 1 (the raw form gave
   inc dword ptr [mem]). Residual: none. */
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
typedef unsigned int undefined4;
MGLOBAL(int, DAT_006bf1dc, 0x006bf1dc);
MGLOBAL(int, DAT_006bf1d8, 0x006bf1d8);
// MATCH: 0x0047a7d0 FUN_0047a7d0

undefined4 FUN_0047a7d0(void)

{
  int i;
  i = DAT_006bf1dc;
  *(undefined4 *)(DAT_006bf1d8 + 0x159c0 + i * 4) = 0xffffffff;
  DAT_006bf1dc = i + 1;
  return 0;
}
