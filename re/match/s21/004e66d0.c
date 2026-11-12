// TOOLCHAIN rw9044
// RWC-MATCH-S02: 100% under rw9044 with return clump inside the loop in place of break (same exits, same value).
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004e66d0 (h4): 70.6% (regs 76.5%). RpClumpForAllAtomics, same ForAll form as the S19 100% 0x004c59c0 (next read before the callback, break on NULL). Residual: ours keeps clump in ecx and tail-duplicates the empty-list return (mov eax,ecx; pops); the original computes the terminator from eax, overwrites it with cur, and its empty-list je goes to the shared epilogue that reloads clump from [esp+0x14]. Tried: cur/end order, end->next, Ghidra's do-while form (42.9), permuter 92 compiles 0 gains. Class (b). */
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
typedef struct RwLLLink { struct RwLLLink *next; struct RwLLLink *prev; } RwLLLink;
typedef struct RwLinkList { RwLLLink link; } RwLinkList;
typedef struct RpClump { int object[2]; RwLinkList atomicList; } RpClump;
typedef struct RpAtomic RpAtomic;
typedef RpAtomic *(*RpAtomicCallBack)(RpAtomic *atomic, void *data);
// MATCH: 0x004e66d0 FUN_004e66d0
RpClump *FUN_004e66d0(RpClump *clump, RpAtomicCallBack callback, void *pData)
{
  RwLLLink *cur = clump->atomicList.link.next;
  RwLLLink *end = &clump->atomicList.link;
  while (cur != end) {
    RwLLLink *next = cur->next;
    if (!callback((RpAtomic *)((char *)cur - 0x40), pData)) return clump;
    cur = next;
  }
  return clump;
}
