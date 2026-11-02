// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c0c20 (h1): 98.9% (regs 100%, ops 100%), relocheck OK. RwFrameDestroy shape: plugin deinit
   FUN_004d8060, RwFrameRemoveChild inlined (sibling-list unlink, root reset, FUN_004c0ad0 walk, RwFrameUpdateObjects
   dirty-list insert with the flags byte held in a uint), dirty-link unlink, children's parent cleared, free-list free
   through RwGlobals+0x11c. The file holds 0x004c0ad0 as a static callee (the caller keeps ecx/edx live across it).
   FLAGS add /G5 (game flags: 44.7% raw).
   Residual: in the sibling search loop the original loads `next` through the copied register (`mov ecx,eax;
   mov eax,[ecx+0x9c]`), ours through the old one (`[eax+0x9c]`). Tried for/while/assign-in-condition forms and the
   permuter (157 compiles, no gain). Class (b). */
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
typedef void (*RwFreeFn)(void *, void *);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d3e70, 0x007d3e70);
MGLOBALA(char, DAT_00617f78, 4, 0x00617f78);
int FUN_004d8060(void *p0, void *p1);
// MATCH: 0x004c0ad0 FUN_004c0ad0
static void FUN_004c0ad0(RwFrame *frame, RwFrame *root)
{
  RwFrame *child = frame->child;
  frame->root = root;
  for (; child != 0; child = child->next) {
    FUN_004c0ad0(child, root);
  }
}
// MATCH: 0x004c0c20 FUN_004c0c20
int FUN_004c0c20(RwFrame *frame)
{
  RwFrame *parent;
  RwFrame *child;
  unsigned int flags;

  FUN_004d8060(DAT_00617f78, frame);
  parent = (RwFrame *)frame->object.parent;
  if (parent != 0) {
    RwFrame *prev = parent->child;
    if (prev == frame) {
      parent->child = frame->next;
    } else {
      RwFrame *cur;
      for (cur = prev->next; cur != frame; cur = prev->next) {
        prev = cur;
      }
      prev->next = frame->next;
    }
    frame->object.parent = 0;
    frame->next = 0;
    frame->root = frame;
    for (child = frame->child; child != 0; child = child->next) {
      FUN_004c0ad0(child, frame);
    }
    flags = frame->root->object.privateFlags;
    if (!(flags & 3)) {
      frame->root->inDirtyListLink.next = ((RwLLLink *)(DAT_007d3ff8 + 0xbc))->next;
      frame->root->inDirtyListLink.prev = (RwLLLink *)(DAT_007d3ff8 + 0xbc);
      ((RwLLLink *)(DAT_007d3ff8 + 0xbc))->next->prev = &frame->root->inDirtyListLink;
      ((RwLLLink *)(DAT_007d3ff8 + 0xbc))->next = &frame->root->inDirtyListLink;
    }
    frame->root->object.privateFlags = (unsigned char)(flags | 3);
    frame->object.privateFlags |= 0xc;
  }
  if (frame->object.privateFlags & 3) {
    frame->inDirtyListLink.prev->next = frame->inDirtyListLink.next;
    frame->inDirtyListLink.next->prev = frame->inDirtyListLink.prev;
  }
  for (child = frame->child; child != 0; child = child->next) {
    child->object.parent = 0;
  }
  (*(RwFreeFn *)(DAT_007d3ff8 + 0x11c))(*(void **)(DAT_007d3ff8 + DAT_007d3e70), frame);
  return 1;
}
