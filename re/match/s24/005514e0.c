// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x005514e0 (h2): 100% (regs 100%), byte-identical (nosize.py: no Ghidra size; the symbol is 40 B
   and ends at 0x00551508, then 90 padding to the next entry 0x00551510). The three fields are read into locals
   before the first compare (inline form 66.7%: the loads sank into the branches). */
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
// MATCH: 0x005514e0 FUN_005514e0
int FUN_005514e0(int *param_1)
{
  int pos_lo = param_1[2];
  int pos_hi = param_1[3];
  int size_hi = param_1[1];

  if (((pos_lo != -1) && (pos_hi <= size_hi)) &&
     ((pos_hi != size_hi || (pos_lo < *param_1)))) {
    return 0;
  }
  return 1;
}
