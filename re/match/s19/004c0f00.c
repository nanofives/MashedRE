// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c0f00 (h1): 98.8% (regs 100%, ops 100%), relocheck OK. RwFrameAddChild shape: RwFrameRemoveChild
   inlined (same text as in 0x004c0c20), then link as first child, parent pointer, root taken from the parent into a
   local and pushed down through FUN_004c0ad0. The file holds 0x004c0ad0 as a static callee. FLAGS add /G5 (game
   flags: 57.5% raw). Residual: the same sibling-search loop base register as 0x004c0c20 (`[eax+0x9c]` vs
   `[edx+0x9c]`). Tried: for/while forms, a static __inline remove helper (not inlined under /Ob1: 26%). h6 resume: do-while, for(;;)-break, temp-next, RW while(cur->next!=child), __inline unlink/find-prev helpers, flags /G6 /Op /Ox /Oy-, permuter again (0c20 157, 0f00 174 compiles): none moves it. Class (b). */
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
typedef struct RwObject { char type; char subType; unsigned char flags; unsigned char privateFlags; void *parent; } RwObject;
typedef struct RwFrame {
  RwObject object;
  RwLLLink inDirtyListLink;
  float modelling[16];
  float ltm[16];
  RwLLLink objectList;
  struct RwFrame *child;
  struct RwFrame *next;
  struct RwFrame *root;
} RwFrame;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
// MATCH: 0x004c0ad0 FUN_004c0ad0
static void FUN_004c0ad0(RwFrame *frame, RwFrame *root)
{
  RwFrame *child = frame->child;
  frame->root = root;
  for (; child != 0; child = child->next) {
    FUN_004c0ad0(child, root);
  }
}
// MATCH: 0x004c0f00 FUN_004c0f00
RwFrame *FUN_004c0f00(RwFrame *parent, RwFrame *child)
{
  RwFrame *oldParent;
  RwFrame *c;
  unsigned int flags;
  RwFrame *root;

  oldParent = (RwFrame *)child->object.parent;
  if (oldParent != 0) {
    RwFrame *prev = oldParent->child;
    if (prev == child) {
      oldParent->child = child->next;
    } else {
      RwFrame *cur;
      for (cur = prev->next; cur != child; cur = prev->next) {
        prev = cur;
      }
      prev->next = child->next;
    }
    child->object.parent = 0;
    child->next = 0;
    child->root = child;
    for (c = child->child; c != 0; c = c->next) {
      FUN_004c0ad0(c, child);
    }
    flags = child->root->object.privateFlags;
    if (!(flags & 3)) {
      child->root->inDirtyListLink.next = ((RwLLLink *)(DAT_007d3ff8 + 0xbc))->next;
      child->root->inDirtyListLink.prev = (RwLLLink *)(DAT_007d3ff8 + 0xbc);
      ((RwLLLink *)(DAT_007d3ff8 + 0xbc))->next->prev = &child->root->inDirtyListLink;
      ((RwLLLink *)(DAT_007d3ff8 + 0xbc))->next = &child->root->inDirtyListLink;
    }
    child->root->object.privateFlags = (unsigned char)(flags | 3);
    child->object.privateFlags |= 0xc;
  }
  child->next = parent->child;
  parent->child = child;
  child->object.parent = parent;
  root = parent->root;
  child->root = root;
  for (c = child->child; c != 0; c = c->next) {
    FUN_004c0ad0(c, root);
  }
  return parent;
}
