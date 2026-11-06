// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 h2 0x0048ebc0: 100% (regs 100%), whole symbol byte-identical. Raw converter had a longlong divide (23%); a plain int divide by the record field (record stride 0x488 = 0x122 dwords, field at 0x0076d994) and n >= 8 clamp match. Residual: none. */
/* ---- MATCH-S15 preamble (same text in every re/match/s15 source; not part of any function body) ----
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
MGLOBALA(int, DAT_0076d994, 0x122, 0x0076d994);
// MATCH: 0x0048ebc0 FUN_0048ebc0
int FUN_0048ebc0(int param_1)
{
  int n = 8 / DAT_0076d994[param_1 * 0x122];
  if (n >= 8) {
    n = 8;
  }
  return n;
}
