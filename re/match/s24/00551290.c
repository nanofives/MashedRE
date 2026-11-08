// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x00551290 (h2): 100% (regs 100%), byte-identical (nosize.py: no Ghidra size; the symbol is 23 B
   and ends at 0x005512a7, then 90 padding to the next entry 0x005512b0). Tail call through RwGlobals
   (DAT_007d3ff8) slot +0x10c with *(param_1+0x58) as the one stack argument. */
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
typedef void (*fn10c_t)(int);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
// MATCH: 0x00551290 FUN_00551290
void FUN_00551290(int param_1)
{
  (*(fn10c_t *)(DAT_007d3ff8 + 0x10c))(*(int *)(param_1 + 0x58));
}
