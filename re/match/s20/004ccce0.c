// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004ccce0 (h3): 100% (regs 100%). RwFreeListForAllUsed shape: per block, RwMalloc (RwGlobals+0x108, hint 0x10000) a copy of the heapSize-byte free map (memcpy intrinsic), read next, walk set bits calling fpCallBack(aligned entry, data), RwFree (+0x10c). Moved it from 33.6%: RwFreeList/RwLLLink structs, the map byte held in an unsigned int and the bit test done on (unsigned char) operands (test dl,al). Relocheck OK. */
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
void *memcpy(void *d, const void *s, unsigned int n);
#pragma intrinsic(memcpy)
typedef struct RwLLLink { struct RwLLLink *next; struct RwLLLink *prev; } RwLLLink;
typedef struct RwFreeList {
  unsigned int entrySize;
  unsigned int entriesPerBlock;
  unsigned int heapSize;
  unsigned int alignment;
  RwLLLink blockList;
} RwFreeList;
typedef void (*RwFreeListCallBack)(void *mem, void *data);
typedef void *(*fpMalloc)(unsigned int size, unsigned int hint);
typedef void (*fpFree)(void *mem);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);

// MATCH: 0x004ccce0 FUN_004ccce0
RwFreeList *FUN_004ccce0(RwFreeList *freelist, RwFreeListCallBack fpCallBack, void *pData)
{
  RwLLLink *cur;
  RwLLLink *next;
  unsigned int heapSize;
  unsigned char *freeMap;
  unsigned int i;
  unsigned int j;
  unsigned int bits;

  heapSize = freelist->heapSize;
  cur = freelist->blockList.next;
  while (cur != &freelist->blockList) {
    freeMap = (unsigned char *)(*(fpMalloc *)(DAT_007d3ff8 + 0x108))(heapSize, 0x10000);
    if (!freeMap) {
      return 0;
    }
    memcpy(freeMap, cur + 1, heapSize);
    next = cur->next;
    for (i = 0; i < heapSize; i++) {
      bits = freeMap[i];
      if (bits) {
        for (j = 0; j < 8; j++) {
          if ((unsigned char)bits & (unsigned char)(0x80 >> j)) {
            fpCallBack((void *)((((unsigned int)cur + freelist->alignment + heapSize + 7) & ~(freelist->alignment - 1))
                                + (j + i * 8) * freelist->entrySize), pData);
          }
        }
      }
    }
    (*(fpFree *)(DAT_007d3ff8 + 0x10c))(freeMap);
    cur = next;
  }
  return freelist;
}
