// FLAGS: /O2 /Ob1 /G5
/* RWC-FRAME-CLUMP 2026-11-13 0x004b3f90: written from the disassembly. 0 for a null clump, else RpClumpForAllAtomics(clump, 0x004b4070, &out) with out = 0 first (0x004b3f99): the first atomic the walk visits. */
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
int FUN_004b4070(int atomic, int *data);
int FUN_004e66d0(int clump, int (*cb)(int, int *), int *data);
// MATCH: 0x004b3f90 FUN_004b3f90
int FUN_004b3f90(int clump)
{
  int atomic = 0;
  if (clump != 0) FUN_004e66d0(clump, FUN_004b4070, &atomic);
  return atomic;
}
