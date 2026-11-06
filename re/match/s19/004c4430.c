// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c4430 (h5): 100% (regs 100%). Matrix module close: destroy the module free list (FUN_004cc9f0) if
   set and clear it, numInstances (DAT_007d402c) -= 1, return instance. RwGlobals pointer first.
   Size: Ghidra has none, so match.py refuses it; scored with the odis.py size 61 B (0x004c4430..0x004c446c, `ret` at
   0x004c446c then nop padding) through match.compare with ghidra_size overridden (h5 scratch msz.py).
   FLAGS: /G5 (RW core build flag, h5 finding); plain /O2 /GS /Ob1 gives 81.2% (load/dec/store vs `dec` scheduling). */
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
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d4028, 0x007d4028);
MGLOBAL(int, DAT_007d402c, 0x007d402c);
int FUN_004cc9f0(void *freeList);

// MATCH: 0x004c4430 FUN_004c4430
void *FUN_004c4430(void *instance, int offset, int size)
{
  if (*(void **)(DAT_007d3ff8 + DAT_007d4028)) {
    FUN_004cc9f0(*(void **)(DAT_007d3ff8 + DAT_007d4028));
    *(void **)(DAT_007d3ff8 + DAT_007d4028) = 0;
  }
  DAT_007d402c--;
  return instance;
}
