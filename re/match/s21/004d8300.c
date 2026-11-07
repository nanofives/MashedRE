// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d8300 (h3): best 51.6% (regs 100%: same instructions, other registers). Raw 45.2%.
   Structs for the frame (+3 flags, +0x90 list head, +0x98 child, +0x9c sibling) and the list-linked object
   (link at +8, callback at +0x10) fix the call form. Residual: the original holds the frame in ebp and pushes
   esi/edi after the NULL test; ours holds it in edi, pushes edi first. Tried a local copy, do/while under if,
   explicit end pointer, callback return type, permuter 180 s (122 compiles, only non-equivalent gains).
   Class (a): callee-saved push shrink-wrapped past the early return (S19 flags brief list). */
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
typedef struct Lnk8300 { struct Lnk8300 *next; struct Lnk8300 *prev; } Lnk8300;
typedef struct Obj8300 { int pad0; int pad4; Lnk8300 link; struct Obj8300 *(*cb)(struct Obj8300 *); } Obj8300;
typedef struct Frm8300 { unsigned char b0, b1, b2, flags; char pad[0x8c]; Lnk8300 list; struct Frm8300 *child; struct Frm8300 *next; } Frm8300;
void FUN_004d8300(Frm8300 *param_1);
// MATCH: 0x004d8300 FUN_004d8300
void FUN_004d8300(Frm8300 *param_1)
{
  Lnk8300 *l;
  Frm8300 *f = param_1;
  while (f != 0) {
    for (l = f->list.next; l != &f->list; l = l->next) {
      Obj8300 *o = (Obj8300 *)((char *)l - 8);
      o->cb(o);
    }
    f->flags &= 0xf7;
    FUN_004d8300(f->child);
    f = f->next;
  }
}
