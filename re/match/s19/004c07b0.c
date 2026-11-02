// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c07b0 (h1): 100% (regs 100%), byte-identical (124 B; no Ghidra size in pc_funcs.csv, scored with
   the size passed in by hand), relocheck OK. RW module-open shape (_rwFrameOpen style): globalsOffset =
   DAT_007d3e70, numInstances = DAT_007d3e74 (RwModuleInfo, written as two MGLOBALs so relocheck resolves the +4),
   free list created by FUN_004cc9e0 into the per-module globals slot, then rwLinkListInitialize on RwGlobals+0xbc.
   FLAGS add /G5: the game flags give 71.9% (`inc dword ptr [g]` instead of load/inc/store, cmp-with-memory
   instead of load/test). /G5 gives 100%. */
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
MGLOBAL(int, DAT_00617f78, 0x00617f78);
MGLOBAL(int, DAT_00617f90, 0x00617f90);
MGLOBAL(int, DAT_00617f94, 0x00617f94);
MGLOBAL(int, DAT_007d3e70, 0x007d3e70); /* RwModuleInfo.globalsOffset */
MGLOBAL(int, DAT_007d3e74, 0x007d3e74); /* RwModuleInfo.numInstances */
MGLOBALA(char, DAT_007d3e78, 4, 0x007d3e78);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
void *FUN_004cc9e0(int entrySize, int entriesPerBlock, int alignment, int numBlocks, void *inPlace, int hint);
// MATCH: 0x004c07b0 FUN_004c07b0
void *FUN_004c07b0(void *instance, int offset, int size)
{
  DAT_007d3e70 = offset;
  *(void **)(DAT_007d3ff8 + DAT_007d3e70) =
      FUN_004cc9e0(DAT_00617f78, DAT_00617f90, 4, DAT_00617f94, DAT_007d3e78, 0x4000e);
  if (*(void **)(DAT_007d3ff8 + DAT_007d3e70) == 0) {
    return 0;
  }
  *(int *)(DAT_007d3ff8 + 0xbc) = DAT_007d3ff8 + 0xbc;
  *(int *)(DAT_007d3ff8 + 0xc0) = DAT_007d3ff8 + 0xbc;
  DAT_007d3e74++;
  return instance;
}
