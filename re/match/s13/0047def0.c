// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x0047def0 (h6), 2738 B (Ghidra size; the body runs to 0x0047e9be): best 85.3% (regs 85.3%,
   ops 98.2%) by hand from the decomp (raw g2s did not compile). Permuter: 1445 compiles, its only gain
   (90.5%) moved `data[2] = DAT_0086cba4[id].f1c` above `id = FUN_0047ce80(id)`, which changes the
   semantics, so it is not kept.
   What moved it: the event ring 0x007e9de0..0x007ec9e4 is ONE struct (count, type[256], stamp[256],
   who[256], data[256*8], mat[256*16]); as separate arrays cl hoists the count, the original reloads it after
   every store. The push is written out at each of the 6 sites (a macro here; a static __inline function
   gives the same code), its copy loops are do-while (a `for` with a constant count is unrolled by 9176 and
   the original keeps the loops; the 4-iteration player-search loops are do-while for the same reason).
   n->ra / n->rb are 8-byte {obj, ushort id} records copied to locals (gives the dword loads + cmp ax,0xffff
   and the frame slots 0x0c/0x44); ids compare against 0xffff (unsigned, cmp di,0xffff imm16). Branches:
   `if (A && !B) .. else if (!A && B) ..` then a separate `if (!A && !B)` (the original re-tests B after the
   second push). The 8-dword event payload is a union (data[2] is int count/0x0086cbc0 word or float 0.0f).
   Dot product as c2*w2 + (c1*w1 + c0*w0). In both per-car paths the three v[] sums come before the data
   stores.
   Residual (b): (1) the row-copy loops at 0x0047e1e2/0x0047e33f: the original's induction pointer is
   node+0x14 (z of item 0, reads -8/-4/0), ours node+0x10 (reads -4/0/+4); tried x/y/z order, 2-D and flat
   float indexing, V3 row casts, struct copy (worse), pointer-offset spellings; nothing moved it. Its byte
   length difference shifts the loop-alignment padding and every later branch target, which is most of the
   remaining 15%. (2) in the c==2 path one store (data[2] = 0) is scheduled one slot early.
   Unmatched source traits kept as the original shows them: v[] compares x,y,z against 0x005d757c, clamp
   t>1 -> 1 else t<0 -> 0 against 0x005cc320 / 0x005d757c. Callees all cdecl; FUN_0055ae50 returns short. */
/* ---- MATCH-S13 preamble (same text in every re/match/s13 source; not part of any function body) ----
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
typedef struct Obj_def0 {
  unsigned char pad0[0x20];
  unsigned short id;            /* +0x20 */
  unsigned short pad22;
  int f24;                      /* +0x24 */
} Obj_def0;
typedef struct V3_def0 { float x, y, z; } V3_def0;
typedef struct Item_def0 { V3_def0 p; unsigned char pad[0x1c]; } Item_def0;   /* stride 0x28 */
typedef struct Ref_def0 { Obj_def0 *obj; unsigned short id; short pad; } Ref_def0;
typedef struct Node_def0 {
  V3_def0 pos;                  /* +0x00 */
  Item_def0 items[4];           /* +0x0c */
  int count;                    /* +0xac */
  float *vec;                   /* +0xb0 */
  Ref_def0 ra;                  /* +0xb4 */
  Ref_def0 rb;                  /* +0xbc */
  unsigned char padc4[0xdc - 0xc4];
  struct Node_def0 *next;       /* +0xdc */
} Node_def0;
typedef struct Bin_def0 { int f0; Node_def0 *head; unsigned char pad[0x20]; } Bin_def0;  /* stride 0x28 */
typedef struct Bucket_def0 {
  unsigned char pad0[0xc8];
  unsigned int count;           /* +0xc8 */
  unsigned char padcc[8];
  Bin_def0 *bins;               /* +0xd4 */
} Bucket_def0;
typedef struct CarLen_def0 { float len; unsigned char pad[0xd00]; } CarLen_def0;  /* stride 0xd04 */
typedef struct Spr_def0 { float f0; unsigned char pad4[0x18]; int f1c; unsigned char pad20[0xf0]; } Spr_def0;  /* stride 0x110 */
typedef struct Res_def0 { unsigned char pad[0x10]; int *f10; } Res_def0;
typedef struct EvRing_def0 {
  unsigned int n;               /* 0x007e9de0 */
  int type[256];                /* 0x007e9de4 */
  int stamp[256];               /* 0x007ea1e4 */
  int who[256];                 /* 0x007ea5e4 */
  int data[256 * 8];             /* 0x007ea9e4 */
  float mat[256 * 16];           /* 0x007ec9e4 */
} EvRing_def0;
MGLOBAL(EvRing_def0, DAT_007e9de0, 0x007e9de0);
MGLOBAL(int, DAT_007f1030, 0x007f1030);
MGLOBALA(int, DAT_006c9a78, 4, 0x006c9a78);
MGLOBALA(int, DAT_006c71d8, 1, 0x006c71d8);
MGLOBAL(unsigned int, DAT_006c6eb0, 0x006c6eb0);
MGLOBALA(CarLen_def0, DAT_00881f84, 4, 0x00881f84);
MGLOBALA(Spr_def0, DAT_0086cba4, 1, 0x0086cba4);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
void FUN_00426070(void);
Res_def0 *FUN_0057c210(int h);
int FUN_0040e370(unsigned int i);
int FUN_0040e350(void);
short FUN_0055ae50(int a, Obj_def0 *o);
int FUN_0047ce80(int id);
void FUN_0046cb30(float *v, unsigned int i);
void FUN_0046bf50(float *v, unsigned int i);
float FUN_004c39b0(float *out, float *in);

/* the event-ring push is written out at every site in the original (inlined; a macro here) */
#define PUSH_EVENT_DEF0(ty, wh) \
  m = FUN_0040e350(); \
  if (m != 1 && m != 2 && m != 3 && m != 4 && m != 5) { \
    DAT_007e9de0.type[DAT_007e9de0.n] = (ty); \
    DAT_007e9de0.stamp[DAT_007e9de0.n] = DAT_007f1030; \
    DAT_007e9de0.who[DAT_007e9de0.n] = (wh); \
    e = 0; \
    do { \
      DAT_007e9de0.data[e + DAT_007e9de0.n * 8] = data.i[e]; \
      e++; \
    } while (e < 8); \
    e = 0; \
    do { \
      DAT_007e9de0.mat[DAT_007e9de0.n * 16 + e] = mat[e]; \
      e++; \
    } while (e < 16); \
    DAT_007e9de0.n++; \
    if (DAT_007e9de0.n >= 0x100) \
      DAT_007e9de0.n = 0; \
  }

// MATCH: 0x0047def0 FUN_0047def0
void FUN_0047def0(Bucket_def0 *param_1)
{
  float v[3];
  Ref_def0 rb;
  unsigned int k;
  float w[3];
  union { int i[8]; float f[8]; } data;
  Ref_def0 ra;
  float mat[16];
  int active[4];
  Obj_def0 *pl[4];
  float cv[3];
  unsigned int i;
  unsigned int j;
  int found;
  int c;
  int flag;
  int id;
  float t;
  Node_def0 *n;
  Res_def0 *r;
  int m;
  int e;

  FUN_00426070();
  for (i = 0; i < 4; i++) {
    pl[i] = (Obj_def0 *)FUN_0057c210(DAT_006c9a78[i]);
    active[i] = FUN_0040e370(i);
  }
  for (k = 0; k < param_1->count; k++) {
    for (n = param_1->bins[k].head; n != 0; n = n->next) {
      ra = n->ra;
      rb = n->rb;
      if (ra.id == 0xffff && rb.id != 0xffff) {
        if (FUN_0055ae50(rb.obj->f24, rb.obj) == 6) {
          id = rb.obj->id;
          data.i[0] = id;
          data.i[1] = (int)rb.obj;
          PUSH_EVENT_DEF0(9, id)
        }
      } else if (ra.id != 0xffff && rb.id == 0xffff) {
        if (FUN_0055ae50(ra.obj->f24, ra.obj) == 6) {
          id = ra.obj->id;
          data.i[0] = id;
          data.i[1] = (int)ra.obj;
          PUSH_EVENT_DEF0(9, id)
        }
      }
      if (ra.id != 0xffff && rb.id != 0xffff) {
        if (FUN_0055ae50(ra.obj->f24, ra.obj) == 7) {
          data.i[0] = ra.obj->id;
          data.i[1] = (int)ra.obj;
          found = -1;
          i = 0;
          do {
            if (active[i] != 0 && rb.obj == pl[i])
              found = i;
            i++;
          } while (i < 4);
          if (found != -1) {
            for (j = 0; (int)j < n->count; j++) {
              mat[j * 3 + 0] = n->items[j].p.x;
              mat[j * 3 + 1] = n->items[j].p.y;
              mat[j * 3 + 2] = n->items[j].p.z;
            }
            data.i[2] = n->count;
            mat[12] = -n->pos.x;
            mat[13] = -n->pos.y;
            mat[14] = -n->pos.z;
            PUSH_EVENT_DEF0(10, found + 0xff00)
          }
        } else if (FUN_0055ae50(rb.obj->f24, rb.obj) == 7) {
          data.i[0] = rb.obj->id;
          data.i[1] = (int)rb.obj;
          found = -1;
          i = 0;
          do {
            if (active[i] != 0 && ra.obj == pl[i])
              found = i;
            i++;
          } while (i < 4);
          if (found != -1) {
            for (j = 0; (int)j < n->count; j++) {
              mat[j * 3 + 0] = n->items[j].p.x;
              mat[j * 3 + 1] = n->items[j].p.y;
              mat[j * 3 + 2] = n->items[j].p.z;
            }
            mat[12] = n->pos.x;
            mat[13] = n->pos.y;
            mat[14] = n->pos.z;
            data.i[2] = n->count;
            PUSH_EVENT_DEF0(10, found + 0xff00)
          }
        }
        for (i = 0; i < 4; i++) {
          if (active[i] == 0)
            continue;
          c = 0;
          if (ra.obj == pl[i])
            c = 1;
          if (rb.obj == pl[i])
            c = 2;
          else if (c == 0)
            continue;
          flag = 0;
          if (c == 1) {
            j = 0;
            do {
              if (active[j] != 0 && rb.obj == pl[j])
                flag = 1;
              j++;
            } while (j < 4);
            if (flag)
              continue;
            v[0] = n->vec[0] + n->vec[0];
            v[1] = n->vec[1] + n->vec[1];
            v[2] = n->vec[2] + n->vec[2];
            data.i[0] = i;
            data.f[2] = 0.0f;
            mat[0] = 1.0f;
            mat[1] = DAT_00881f84[i].len;
            id = rb.obj->id;
            if ((unsigned int)id < DAT_006c6eb0) {
              data.i[1] = id;
              r = FUN_0057c210(DAT_006c71d8[id]);
              if (r != 0 && r->f10[1] != 0) {
                id = FUN_0047ce80(id);
                data.i[2] = DAT_0086cba4[id].f1c;
                mat[0] = DAT_0086cba4[id].f0;
              }
              mat[9] = n->items[0].p.x;
              mat[10] = n->items[0].p.y;
              mat[11] = n->items[0].p.z;
              mat[12] = n->pos.x;
              mat[13] = n->pos.y;
              mat[14] = n->pos.z;
              PUSH_EVENT_DEF0(8, i + 0xff00)
            }
          } else if (c == 2) {
            j = 0;
            do {
              if (active[j] != 0 && ra.obj == pl[j])
                flag = 1;
              j++;
            } while (j < 4);
            if (flag)
              continue;
            v[0] = n->vec[8] + n->vec[8];
            v[1] = n->vec[9] + n->vec[9];
            v[2] = n->vec[10] + n->vec[10];
            data.f[2] = 0.0f;
            data.i[0] = i;
            mat[0] = 1.0f;
            mat[1] = DAT_00881f84[i].len;
            id = ra.obj->id;
            if ((unsigned int)id < DAT_006c6eb0) {
              data.i[1] = id;
              r = FUN_0057c210(DAT_006c71d8[id]);
              if (r != 0 && r->f10[1] != 0) {
                id = FUN_0047ce80(id);
                data.i[2] = DAT_0086cba4[id].f1c;
                mat[0] = DAT_0086cba4[id].f0;
              }
              mat[9] = n->items[0].p.x;
              mat[10] = n->items[0].p.y;
              mat[11] = n->items[0].p.z;
              mat[12] = n->pos.x;
              mat[13] = n->pos.y;
              mat[14] = n->pos.z;
              PUSH_EVENT_DEF0(8, i + 0xff00)
            }
          }
          FUN_0046cb30(w, i);
          if (v[0] != DAT_005d757c && v[1] != DAT_005d757c && v[2] != DAT_005d757c &&
              w[0] != DAT_005d757c && w[1] != DAT_005d757c && w[2] != DAT_005d757c) {
            FUN_004c39b0(cv, v);
            FUN_004c39b0(w, w);
            t = DAT_005cc320 - (cv[2] * w[2] + (cv[1] * w[1] + cv[0] * w[0]));
            if (t > DAT_005cc320)
              t = DAT_005cc320;
            else if (t < DAT_005d757c)
              t = DAT_005d757c;
            v[0] = v[0] * t;
            v[1] = v[1] * t;
            v[2] = v[2] * t;
          }
          FUN_0046bf50(v, i);
        }
      }
    }
  }
}
