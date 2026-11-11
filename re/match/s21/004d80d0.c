// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d80d0 (h1, by hand): best 91.4% exact, regs 97.8% (raw 34.5%). Dirty-frame list sync (list at
   RwGlobals+0xbc, frame = link - 8): hierarchy-dirty frames copy modelling to ltm, sync attached objects (callback
   at object+0x10), multiply child LTMs (0x004c4600) and recurse (0x004d8280); others sync objects and recurse via
   0x004d8300; flags &= 0xf0; then the list is re-initialised; returns 1. Object sync loop is a local macro (#undef
   after). Residual: scratch-register choice at three spots (matrix-multiply argument setup eax/ecx/edx rotated,
   child->child load into eax after the flag store instead of ecx before it, ecx/edx in the else-branch call and
   the list init). Tried: argument temporaries, child->child hoisted, int vs uint flags, signed RwObject bytes,
   RwGlobals as a struct with the list member, alternate list-init spellings, permuter (164 compiles, no gain).
   Class (a). C++ ok. */
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
typedef struct { float m[16]; } RwMatrix_;
typedef struct RwLLLink_ RwLLLink_;
struct RwLLLink_ { RwLLLink_ *next; RwLLLink_ *prev; };
typedef struct RwFrame_ RwFrame_;
struct RwFrame_ {
  uchar type;
  uchar subType;
  uchar flags;
  uchar privateFlags;
  RwFrame_ *parent;
  RwLLLink_ inDirtyListLink;
  RwMatrix_ modelling;
  RwMatrix_ ltm;
  RwLLLink_ objectList;
  RwFrame_ *child;
  RwFrame_ *next;
};
typedef struct FUN_004d80d0_obj FUN_004d80d0_obj;
struct FUN_004d80d0_obj {
  int object[2];
  RwLLLink_ lFrame;
  FUN_004d80d0_obj *(*sync)(FUN_004d80d0_obj *obj);
};
MGLOBAL(char *, DAT_007d3ff8, 0x007d3ff8);
RwMatrix_ *FUN_004c4600(RwMatrix_ *dst, RwMatrix_ *a, RwMatrix_ *b);
void FUN_004d8280(RwFrame_ *frame, uint flags);
void FUN_004d8300(RwFrame_ *frame);
#define FUN_004d80d0_SYNC(f)                                                      \
  do {                                                                            \
    RwLLLink_ *cur_, *end_;                                                       \
    end_ = &(f)->objectList;                                                      \
    for (cur_ = end_->next; cur_ != end_; cur_ = cur_->next) {                    \
      FUN_004d80d0_obj *o_ = (FUN_004d80d0_obj *)((char *)cur_ - 8);              \
      o_->sync(o_);                                                               \
    }                                                                             \
  } while (0)
// MATCH: 0x004d80d0 FUN_004d80d0
int FUN_004d80d0(void)

{
  RwLLLink_ *cur;
  RwLLLink_ *end;
  RwFrame_ *frame;
  RwFrame_ *child;
  uint flags;
  uint subtree;
  uint childFlags;

  end = (RwLLLink_ *)(DAT_007d3ff8 + 0xbc);
  cur = end->next;
  while (cur != end) {
    frame = (RwFrame_ *)((char *)cur - 8);
    flags = frame->privateFlags;
    if (flags & 1) {
      subtree = flags & 4;
      if (subtree) {
        frame->ltm = frame->modelling;
      }
      FUN_004d80d0_SYNC(frame);
      for (child = frame->child; child != 0; child = child->next) {
        childFlags = child->privateFlags | subtree;
        if (childFlags & 4) {
          FUN_004c4600(&child->ltm, &child->modelling, &child->parent->ltm);
        }
        FUN_004d80d0_SYNC(child);
        child->privateFlags &= ~0xc;
        FUN_004d8280(child->child, childFlags);
      }
    }
    else {
      FUN_004d80d0_SYNC(frame);
      for (child = frame->child; child != 0; child = child->next) {
        FUN_004d80d0_SYNC(child);
        child->privateFlags &= ~8;
        FUN_004d8300(child->child);
      }
    }
    frame->privateFlags = (uchar)(flags & 0xf0);
    cur = cur->next;
  }
  ((RwLLLink_ *)(DAT_007d3ff8 + 0xbc))->next = (RwLLLink_ *)(DAT_007d3ff8 + 0xbc);
  ((RwLLLink_ *)(DAT_007d3ff8 + 0xbc))->prev = (RwLLLink_ *)(DAT_007d3ff8 + 0xbc);
  return 1;
}
#undef FUN_004d80d0_SYNC
