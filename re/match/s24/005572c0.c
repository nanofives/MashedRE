// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 h3 0x005572c0: 17.9% (regs 38.4%, ops 64.1%), structure only. Recursive ribbon/strip draw over a node list (two vertices per point, batch flush at 0xfe vertices via FUN_004cd070/FUN_004cd170/FUN_004cd140, render states 1/9/2 through RwGlobals+0x20). Residual (c): the original converts the 8 interpolated colour channels with inline fnstcw / or ah,0xc / fldcw / fistp / fldcw sequences (an __asm truncating float-to-int macro, 8 instructions each); plain C (int) casts compile to call __ftol under 13.00.9176 (checked /O2 /Ob1 /G5, /QIfist, /Op, /G6), and __asm is barred by the source rules, so the instruction stream cannot line up. Other open (b) items: node held in ebp in the original (ebx here), render-state values computed before the first call (sunk here), five strength-reduced point pointers vs ours. Tried: pointer walk vs pts[i] indexing, hoisted filter/address temporaries. */
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
typedef struct Pt { float d; float x, y; float nx, ny; float t; } Pt;      /* 0x18 */
typedef struct Vtx { float x, y, z; float pad[3]; unsigned int color; float u, v; } Vtx;  /* 0x24 */
typedef struct Node Node;
struct Node {
  int pts;              /* +0x00 */
  int count;            /* +0x04 */
  int pad08;
  int built;            /* +0x0c */
  float offset;         /* +0x10 */
  Node *next;           /* +0x14 */
};
typedef struct Tex { int raster; int pad[19]; unsigned int flags; } Tex;  /* +0x50 */
typedef struct Style {
  float c0[4];          /* +0x00 */
  float uv0[2];         /* +0x10 */
  float dc0[4];         /* +0x18 */
  float duv0[2];        /* +0x28 */
  float c1[4];          /* +0x30 */
  float uv1[2];         /* +0x40 */
  float dc1[4];         /* +0x48 */
  float duv1[2];        /* +0x58 */
  unsigned char rgba[4];/* +0x60 */
  unsigned char flags;  /* +0x64 */
  char pad65[3];
  Tex *tex;             /* +0x68 */
  int pad6c;
  float width;          /* +0x70 */
} Style;
typedef int (__cdecl *rsset_t)(int, int);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBALA(unsigned short, DAT_00912500, 256, 0x00912500);
MGLOBAL(Vtx *, DAT_00912a04, 0x00912a04);
MGLOBAL(int, DAT_00912bdc, 0x00912bdc);
MGLOBAL(float, DAT_00912be0, 0x00912be0);
int __cdecl FUN_004cd070(Vtx *, int, int, int);
int __cdecl FUN_004cd140(void);
int __cdecl FUN_004cd170(int, unsigned short *, int);
int __cdecl FUN_00552e40(void);
Node *__cdecl FUN_00553590(void);
Node *__cdecl FUN_00553620(Node *, Node *);
int __cdecl FUN_0055deb0(int);
Pt *__cdecl FUN_005c4d30(int);
// MATCH: 0x005572c0 FUN_005572c0
Node *FUN_005572c0(Node *node, Style *st)
{
  int ctx, count, i, n;
  float z, off, w, a, b, invLen, t;
  Node *src;
  Pt *pts, *p;
  Vtx *v;
  unsigned int fl, addrU, addrV, color, filt, addr;
  float col[4], uv[2];

  if (node) {
    ctx = FUN_00552e40();
    z = DAT_00912be0;
    off = node->offset;
    w = st->width;
    src = node;
    if (!node->built) {
      src = FUN_00553590();
      FUN_00553620(src, node);
    }
    if (src->count == -1) pts = FUN_005c4d30(src->pts);
    else pts = (Pt *)src->pts;
    count = src->count;
    if (count == -1) count = FUN_0055deb0(src->pts);
    if (st->tex) {
      fl = st->tex->flags;
      addrV = (fl >> 12) & 0xf;
      addrU = (fl >> 8) & 0xf;
      filt = fl & 0xff;
      addr = (addrU == addrV) ? addrV : 0;
      (*(rsset_t *)(DAT_007d3ff8 + 0x20))(1, st->tex->raster);
      (*(rsset_t *)(DAT_007d3ff8 + 0x20))(9, filt);
      (*(rsset_t *)(DAT_007d3ff8 + 0x20))(2, addr);
    } else {
      (*(rsset_t *)(DAT_007d3ff8 + 0x20))(1, 0);
    }
    n = 0;
    invLen = 1.0f / pts[count - 1].t;
    v = DAT_00912a04;
    if (count > 0) {
      a = off + w;
      b = w - off;
      for (i = 0; i < count; i++) {
        if (n >= 0xfe) {
          if (FUN_004cd070(DAT_00912a04, n, ctx, (st->tex != 0) | DAT_00912bdc)) {
            FUN_004cd170(4, DAT_00912500, n);
            FUN_004cd140();
          }
          i--;
          n = 0;
          v = DAT_00912a04;
        }
        v[0].x = pts[i].x - pts[i].nx * a;
        v[0].z = z;
        v[0].y = pts[i].y - pts[i].ny * a;
        if (st->flags & 1) {
          t = invLen * pts[i].t;
          col[0] = st->c0[0] + st->dc0[0] * t;
          col[1] = st->c0[1] + st->dc0[1] * t;
          col[2] = st->c0[2] + st->dc0[2] * t;
          col[3] = st->c0[3] + st->dc0[3] * t;
          color = ((((unsigned int)(int)col[3] << 8) | ((int)col[0] & 0xff)) << 8 | ((int)col[1] & 0xff)) << 8 | ((int)col[2] & 0xff);
        } else {
          color = ((((unsigned int)st->rgba[3] << 8) | st->rgba[0]) << 8 | st->rgba[1]) << 8 | st->rgba[2];
        }
        v[0].color = color;
        if (st->flags & 2) {
          t = invLen * pts[i].t;
          uv[0] = st->uv0[0] + st->duv0[0] * t;
          uv[1] = st->uv0[1] + st->duv0[1] * t;
          v[0].u = uv[0];
          v[0].v = uv[1];
        }
        v[1].x = pts[i].nx * b + pts[i].x;
        v[1].z = z;
        v[1].y = pts[i].y + pts[i].ny * b;
        if (st->flags & 1) {
          t = invLen * pts[i].t;
          col[0] = st->c1[0] + st->dc1[0] * t;
          col[1] = st->c1[1] + st->dc1[1] * t;
          col[2] = st->c1[2] + st->dc1[2] * t;
          col[3] = st->c1[3] + st->dc1[3] * t;
          color = ((((unsigned int)(int)col[3] << 8) | ((int)col[0] & 0xff)) << 8 | ((int)col[1] & 0xff)) << 8 | ((int)col[2] & 0xff);
        } else {
          color = ((((unsigned int)st->rgba[3] << 8) | st->rgba[0]) << 8 | st->rgba[1]) << 8 | st->rgba[2];
        }
        v[1].color = color;
        if (st->flags & 2) {
          t = invLen * pts[i].t;
          uv[0] = st->uv1[0] + st->duv1[0] * t;
          uv[1] = st->uv1[1] + st->duv1[1] * t;
          v[1].u = uv[0];
          v[1].v = uv[1];
        }
        n += 2;
        v += 2;
      }
      if (n > 0 && FUN_004cd070(DAT_00912a04, n, ctx, (st->tex != 0) | DAT_00912bdc)) {
        FUN_004cd170(4, DAT_00912500, n);
        FUN_004cd140();
      }
    }
    FUN_005572c0(node->next, st);
  }
  return node;
}
