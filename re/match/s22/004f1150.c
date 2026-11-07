// FLAGS: /O2 /Ob1 /G5
/* MATCH-S22 0x004f1150 (h1): best 67.6% (regs 73.0%, ops 78.0%, 224/259 ins) under /O2 /Ob1 /G5 (flag sweep: no
   set better). Raw 39.8%. Not a register-arg function: Ghidra's unaff_EBP is the stream (arg 1, [esp+0xc0]), the
   object is arg 2. Stream writer: if obj+8 has bit 0x1000000, s = obj->g + 0x18; size = g+8 + 8 + n*8 + 4 (n from
   iface vtbl+0x10 at s+0x34) [+ 2*s+0x38 when s+8] + per entry (s+0x10, 2 x 16 B) 0x10 [+ cnt*stride]; remaps the
   s+4 items (stride 0x24 at s+0x48) through 0x004f3cb0(obj+0x20); writes chunk header 0x004cc580(stream, 1, size,
   0x37002, 10), 0x006189fc, g+8, g+0x18 (g+8 bytes), n, the n*8 buffer, the locked index data (vtbl+0x2c/+0x30),
   both entries and their locked data, then maps the items back through 0x004f3bd0. The size loop as an indexed
   do/while (same form that matched 0x004f1800) moved it 47.9 -> 67.6 and brought the 0-in-ebx index-lock section.
   Residuals: (1) the original keeps six separate `pop x4; add esp; ret` exits after the six early write tests
   (eax already 0, no xor), 9176 tail-merges them into one exit (36 instructions; same as the S19 tail-merge
   residuals in 0x004c4a50/0x004c6750; `ok = f(); if (!ok) return ok;` did not move it); (2) ours caches obj in a
   register early, the original re-reads [esp+0xc4] and loads obj into edi only after the chunk header; (3) the
   original gives the index-lock pointer and the entry loop counter one slot and the entry-lock pointer another
   (a separate ibptr variable scores 63.3%). Class (b) (budget spent; (1) alone is compiler-version class). */
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
typedef struct IfVt_004f1150 IfVt_004f1150;
typedef struct { IfVt_004f1150 *vt; } If_004f1150;
struct IfVt_004f1150 {
  void *s0[4];
  int (__stdcall *GetInfo)(If_004f1150 *self, void *buf, int *n);
  void *s1[6];
  int (__stdcall *Lock)(If_004f1150 *self, int off, int size, void **ptr, int flags);
  int (__stdcall *Unlock)(If_004f1150 *self);
};
typedef struct { If_004f1150 *p; int off; int cnt; int pad; } Ent_004f1150;
typedef struct {
  int x0; int count; If_004f1150 *ib; int pad2;
  Ent_004f1150 e[2]; int pad3; If_004f1150 *iface; int ibcount; int stride; char pad4[8]; int items[1];
} S_004f1150;
typedef struct { char pad0[8]; int size; char pad1[0xc]; S_004f1150 s; } G_004f1150;
typedef struct { char pad0[8]; unsigned int flags; char pad1[0x14]; char sub[0x38]; G_004f1150 *g; } Obj_004f1150;
MGLOBAL(int, DAT_006189fc, 0x006189fc);
void *FUN_004cbe80(void *stream, const void *buf, unsigned int len);
void *FUN_004cc580(void *stream, int type, int size, int version, int build);
int FUN_004f3bd0(void *p0, int p1);
int FUN_004f3cb0(void *p0, int p1);
// MATCH: 0x004f1150 FUN_004f1150
void *FUN_004f1150(void *stream, Obj_004f1150 *obj)
{
  S_004f1150 *s;
  int size;
  int n;
  unsigned int i;
  int *it;
  void *ptr;
  int buf[40];
  Ent_004f1150 *e;
  void *ok;

  if (obj->flags & 0x1000000) {
    s = &obj->g->s;
    size = obj->g->size + 8;
    s->iface->vt->GetInfo(s->iface, buf, &n);
    size += n * 8 + 4;
    if (s->ib) {
      size += s->ibcount * 2;
    }
    i = 0;
    do {
      size += sizeof(Ent_004f1150);
      if (s->e[i].p) {
        size += s->stride * s->e[i].cnt;
      }
    } while (++i < 2);
    i = s->count;
    if (i) {
      it = s->items;
      do {
        *it = FUN_004f3cb0(obj->sub, *it);
        it += 9;
      } while (--i);
    }
    if (!FUN_004cc580(stream, 1, size, 0x37002, 10)) {
      return 0;
    }
    if (!FUN_004cbe80(stream, &DAT_006189fc, 4)) {
      return 0;
    }
    if (!FUN_004cbe80(stream, &obj->g->size, 4)) {
      return 0;
    }
    if (!FUN_004cbe80(stream, &obj->g->s, obj->g->size)) {
      return 0;
    }
    if (!FUN_004cbe80(stream, &n, 4)) {
      return 0;
    }
    if (!FUN_004cbe80(stream, buf, n * 8)) {
      return 0;
    }
    if (s->ib) {
      ptr = 0;
      s->ib->vt->Lock(s->ib, 0, 0, &ptr, 0);
      if (!ptr) {
        return 0;
      }
      ok = FUN_004cbe80(stream, ptr, s->ibcount * 2);
      s->ib->vt->Unlock(s->ib);
      if (!ok) {
        return 0;
      }
    }
    e = s->e;
    for (i = 0; i < 2; i++, e++) {
      if (!FUN_004cbe80(stream, e, sizeof(Ent_004f1150))) {
        return 0;
      }
      if (e->p) {
        if (e->p->vt->Lock(e->p, e->off, e->cnt * s->stride, &ptr, 0x800)) {
          return 0;
        }
        ok = FUN_004cbe80(stream, ptr, e->cnt * s->stride);
        e->p->vt->Unlock(e->p);
        if (!ok) {
          return 0;
        }
      }
    }
    i = s->count;
    if (i) {
      it = s->items;
      do {
        *it = FUN_004f3bd0(obj->sub, *it);
        it += 9;
      } while (--i);
    }
  }
  return stream;
}
