// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d8350 (h1, by hand): 100.0% exact, regs 100.0% (raw 40.4%). Frame LTM update over the children:
   RwFrame layout (privateFlags +3, parent +4, modelling +0x10, ltm +0x50, child +0x98, next +0x9c) with a 64-byte
   matrix struct copy (rep movsd), RwMatrixMultiply 0x004c4600 and the recursive child walk 0x004d83d0. Moves: real
   struct types and a uint flags (zero-extended, kept in the dead argument slot). relocheck OK, C++ ok. */
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
typedef struct RwFrame_ RwFrame_;
struct RwFrame_ {
  uchar type;
  uchar subType;
  uchar flags;
  uchar privateFlags;
  RwFrame_ *parent;
  int pad08[2];
  RwMatrix_ modelling;
  RwMatrix_ ltm;
  int pad90[2];
  RwFrame_ *child;
  RwFrame_ *next;
};
RwMatrix_ *FUN_004c4600(RwMatrix_ *dst, RwMatrix_ *a, RwMatrix_ *b);
void FUN_004d83d0(RwFrame_ *frame, uint flags);
// MATCH: 0x004d8350 FUN_004d8350
void FUN_004d8350(RwFrame_ *param_1)

{
  uint flags;
  uint childFlags;
  RwFrame_ *child;

  flags = param_1->privateFlags;
  if (flags & 4) {
    param_1->ltm = param_1->modelling;
  }
  for (child = param_1->child; child != 0; child = child->next) {
    childFlags = child->privateFlags | flags;
    if (childFlags & 4) {
      FUN_004c4600(&child->ltm, &child->modelling, &child->parent->ltm);
      child->privateFlags &= ~4;
    }
    FUN_004d83d0(child->child, childFlags);
  }
  param_1->privateFlags = (uchar)(flags & 0xfa);
  return;
}
