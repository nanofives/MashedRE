// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c4470 (h5): 100% (regs 100%). Matrix module open: module offset = offset arg, free list =
   FUN_004cc9e0(0x40, DAT_00618114, 4, DAT_00618118, &DAT_007d4004, 0x4000d) (6-arg cdecl thunk), NULL -> return 0;
   else opts word 0x20000, multiply callback FUN_004c40e0, default tolerance {0.01f x3} struct-copied from a local
   initialised after the opts store, numInstances += 1, return instance.
   Size: Ghidra has none; scored with the odis.py size 191 B (0x004c4470..0x004c452e) through match.compare with
   ghidra_size overridden (h5 scratch msz.py).
   FLAGS: /G5 (RW core build flag, h5 finding); plain /O2 /GS /Ob1 gives 73.3%. */
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
typedef struct RwMatrixTolerance { float Normal; float Orthogonal; float Identity; } RwMatrixTolerance;
typedef void (*fp_4c40e0)(void *, void *, void *);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d4028, 0x007d4028);
MGLOBAL(int, DAT_007d402c, 0x007d402c);
MGLOBAL(int, DAT_00618114, 0x00618114);
MGLOBAL(int, DAT_00618118, 0x00618118);
MGLOBAL(int, DAT_007d4004, 0x007d4004);
void FUN_004c40e0(void *, void *, void *);
void *FUN_004cc9e0(int entrySize, int entriesPerBlock, int alignment, int numBlocksToPrealloc, void *inFreeList, unsigned int hint);

// MATCH: 0x004c4470 FUN_004c4470
void *FUN_004c4470(void *instance, int offset, int size)
{
  RwMatrixTolerance tol;
  DAT_007d4028 = offset;
  *(void **)(DAT_007d3ff8 + DAT_007d4028) = FUN_004cc9e0(0x40, DAT_00618114, 4, DAT_00618118, &DAT_007d4004, 0x4000d);
  if (!*(void **)(DAT_007d3ff8 + DAT_007d4028)) {
    return 0;
  }
  *(unsigned int *)(DAT_007d3ff8 + DAT_007d4028 + 4) = 0x20000;
  tol.Normal = 0.01f;
  tol.Orthogonal = 0.01f;
  tol.Identity = 0.01f;
  *(fp_4c40e0 *)(DAT_007d3ff8 + DAT_007d4028 + 8) = FUN_004c40e0;
  *(RwMatrixTolerance *)(DAT_007d3ff8 + DAT_007d4028 + 0xc) = tol;
  DAT_007d402c++;
  return instance;
}
