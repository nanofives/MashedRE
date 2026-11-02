// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c0830 (h1): 100% (regs 100%), byte-identical (61 B; no Ghidra size in pc_funcs.csv), relocheck OK.
   RW module-close shape (_rwFrameClose style): destroy the per-module free list through FUN_004cc9f0, clear the
   slot, numInstances--. FLAGS add /G5: the game flags give 81.2% (`dec dword ptr [g]` instead of load/dec/store);
   /G5 gives 100%. */
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
MGLOBAL(int, DAT_007d3e70, 0x007d3e70); /* RwModuleInfo.globalsOffset */
MGLOBAL(int, DAT_007d3e74, 0x007d3e74); /* RwModuleInfo.numInstances */
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
void FUN_004cc9f0(void *freeList);
// MATCH: 0x004c0830 FUN_004c0830
void *FUN_004c0830(void *instance, int offset, int size)
{
  if (*(void **)(DAT_007d3ff8 + DAT_007d3e70) != 0) {
    FUN_004cc9f0(*(void **)(DAT_007d3ff8 + DAT_007d3e70));
    *(void **)(DAT_007d3ff8 + DAT_007d3e70) = 0;
  }
  DAT_007d3e74--;
  return instance;
}
