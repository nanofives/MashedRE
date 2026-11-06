// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c0740 (h1): 100% (regs 100%), byte-identical, relocheck OK. RW _rwObjectHasFrameSetFrame shape:
   RwObject.parent at +4, RwObjectHasFrame.lFrame at +8, RwFrame.objectList at +0x90, rwLinkListRemoveLLLink /
   rwLinkListAddLLLink macro order, then FUN_004c0e50(frame).
   FLAGS add /G5: under the game flags (/O2 /GS /Ob1) the best form was 90% (call-argument push scheduled after the
   lFrame.prev store; permuter 115 variants found nothing). /G5 (also /G3, /G4) gives 100%: this RenderWare core code
   was built for Pentium scheduling, not the game's default (/G6 blend in 13.00). */
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
typedef struct RwObject { char type; char subType; char flags; char privateFlags; void *parent; } RwObject;
typedef struct RwFrame { RwObject object; char pad[0x88]; RwLinkList objectList; } RwFrame;
typedef struct RwObjectHasFrame { RwObject object; RwLLLink lFrame; void *sync; } RwObjectHasFrame;
RwFrame *FUN_004c0e50(RwFrame *frame);
// MATCH: 0x004c0740 FUN_004c0740
void FUN_004c0740(void *object, RwFrame *frame)
{
  RwObjectHasFrame *ohf = (RwObjectHasFrame *)object;
  if (((RwObject *)object)->parent) {
    (&ohf->lFrame)->prev->next = (&ohf->lFrame)->next;
    (&ohf->lFrame)->next->prev = (&ohf->lFrame)->prev;
  }
  ((RwObject *)object)->parent = (void *)frame;
  if (frame) {
    (&ohf->lFrame)->next = (&frame->objectList)->link.next;
    (&ohf->lFrame)->prev = &(&frame->objectList)->link;
    ((&frame->objectList)->link.next)->prev = &ohf->lFrame;
    (&frame->objectList)->link.next = &ohf->lFrame;
    FUN_004c0e50(frame);
  }
}
