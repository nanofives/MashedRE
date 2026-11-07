// FLAGS: /O2 /Ob1 /G5
/* MATCH-S22 0x004f1800 (h1): 100% under /O2 /Ob1 /G5. Raw 68.4%. Size of the chunk 0x004f1150 writes: 0 unless obj+8 has bit
   0x1000000, else g+8 + 0x14 + 4 + 2*(g+0x50 + 4n) (n from the iface vtbl+0x10 call at g+0x4c) + per entry (g+0x28, 2 x
   16 B) 0x10 [+ stride*cnt]. Raw form had GetInfo with one argument (it takes buf and &n, stdcall). What fixed the
   last 3 instructions: an indexed do/while (i = 0; ... while (++i < 2)); a pointer walk puts the induction pointer at
   the entry start, the original keeps it at &e->cnt; a for loop unrolls. */
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
typedef struct IfVt_004f1800 IfVt_004f1800;
typedef struct { IfVt_004f1800 *vt; } If_004f1800;
struct IfVt_004f1800 {
  void *s0[4];
  int (__stdcall *GetInfo)(If_004f1800 *self, void *buf, int *n);
};
typedef struct { If_004f1800 *p; int off; int cnt; int pad; } Ent_004f1800;
typedef struct {
  char pad0[8]; int size; char pad1[0x1c];
  Ent_004f1800 e[2]; int pad3; If_004f1800 *iface; int ibcount; int stride;
} G_004f1800;
typedef struct { char pad0[8]; unsigned int flags; char pad1[0x4c]; G_004f1800 *g; } Obj_004f1800;
// MATCH: 0x004f1800 FUN_004f1800
int FUN_004f1800(Obj_004f1800 *obj)
{
  G_004f1800 *g;
  int size;
  int n;
  int buf[40];
  Ent_004f1800 *e;
  int i;

  size = 0;
  if (obj->flags & 0x1000000) {
    g = obj->g;
    size = g->size + 0x14;
    g->iface->vt->GetInfo(g->iface, buf, &n);
    size += (g->ibcount + n * 4) * 2 + 4;
    i = 0;
    do {
      size += sizeof(Ent_004f1800);
      if (g->e[i].p) {
        size += g->stride * g->e[i].cnt;
      }
    } while (++i < 2);
  }
  return size;
}
