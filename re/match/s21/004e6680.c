// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004e6680 (h2): 100% (regs 100%), relocheck OK. Walks the owner's object list (+8), object =
   link - 0x40; when object flags (+2) bit 4 is set, calls FUN_004c0ed0(object->parent +4) then the object's
   callback at +0x48 with the object; any 0 return makes the function return NULL, else the owner.
   What moved it: typed structs and a result variable initialised to the argument (raw form 37.5%). */
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
typedef struct Obj { char type; char subType; char flags; char privateFlags; void *parent; char pad[0x38];
  RwLLLink inList; int (*sync)(struct Obj *); } Obj;
typedef struct Owner { char pad[8]; RwLLLink list; } Owner;
int FUN_004c0ed0(void *frame);
// MATCH: 0x004e6680 FUN_004e6680
Owner *FUN_004e6680(Owner *owner)
{
  Owner *result = owner;
  RwLLLink *cur = owner->list.next;
  RwLLLink *end = &owner->list;
  while (cur != end) {
    Obj *o = (Obj *)((char *)cur - 0x40);
    if (o->flags & 4) {
      FUN_004c0ed0(o->parent);
      if (!o->sync(o)) result = 0;
    }
    cur = cur->next;
  }
  return result;
}
