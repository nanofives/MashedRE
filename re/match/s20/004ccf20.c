// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004ccf20 (h3): 87.7% (regs 88.7%). Empties the block list at 0x007d45cc: per block (link at +0x1c) unlink, unlink+RwFree (+0x10c) every entry of its list at +0x10, then unless flags(+0x18)&1 free it through RwGlobals+0x11c(DAT_007d45fc, blk) or +0x10c; then the same for DAT_007d45fc itself and clears 0x007d45fc / 0x007d45f8. From 59.6%: structs, part 2 written through a link pointer (&DAT_007d45fc->link, blk = link - 0x1c) gave the right block layout (77.4 -> 87.7). Residual: part 2's unlink keeps blk in eax and builds &link.next / &link.prev with lea/add (original) where ours uses link-relative offsets. Tried: blk-relative, link-pointer, mixed per statement, end-pointer local, static __inline helper (not inlined under /Ob1, 7.5%), explicit early-return epilogue, permuter 285 compiles (no gain). Class (b). */
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
typedef struct Blk {
  int pad[4];
  RwLLLink list;
  unsigned int flags;
  RwLLLink link;
} Blk;
typedef void (*fpFree)(void *mem);
typedef void (*fpFree2)(void *heap, void *mem);
MGLOBAL(RwLLLink, DAT_007d45cc, 0x007d45cc);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(Blk *, DAT_007d45fc, 0x007d45fc);
MGLOBAL(int, DAT_007d45f8, 0x007d45f8);

// MATCH: 0x004ccf20 FUN_004ccf20
void FUN_004ccf20(void)
{
  Blk *blk;
  RwLLLink *cur;

  while (DAT_007d45cc.next != &DAT_007d45cc) {
    blk = (Blk *)((char *)DAT_007d45cc.next - 0x1c);
    blk->link.prev->next = blk->link.next;
    blk->link.next->prev = blk->link.prev;
    while ((cur = blk->list.next) != &blk->list) {
      cur->prev->next = cur->next;
      cur->next->prev = cur->prev;
      (*(fpFree *)(DAT_007d3ff8 + 0x10c))(cur);
    }
    if (!(blk->flags & 1)) {
      if (DAT_007d45fc != blk && DAT_007d45fc != 0) {
        (*(fpFree2 *)(DAT_007d3ff8 + 0x11c))(DAT_007d45fc, blk);
      } else {
        (*(fpFree *)(DAT_007d3ff8 + 0x10c))(blk);
      }
    }
  }
  cur = &DAT_007d45fc->link;
  blk = (Blk *)((char *)cur - 0x1c);
  cur->prev->next = cur->next;
  cur->next->prev = cur->prev;
  while ((cur = blk->list.next) != &blk->list) {
    cur->prev->next = cur->next;
    cur->next->prev = cur->prev;
    (*(fpFree *)(DAT_007d3ff8 + 0x10c))(cur);
  }
  if (!(blk->flags & 1)) {
    if (DAT_007d45fc != blk && DAT_007d45fc != 0) {
      (*(fpFree2 *)(DAT_007d3ff8 + 0x11c))(DAT_007d45fc, blk);
      DAT_007d45fc = 0;
      DAT_007d45f8 = 0;
      return;
    }
    (*(fpFree *)(DAT_007d3ff8 + 0x10c))(blk);
  }
  DAT_007d45fc = 0;
  DAT_007d45f8 = 0;
}
