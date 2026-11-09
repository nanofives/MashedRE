// FLAGS: /O2 /GS /Ob1
/* 0x00420050 FUN_00420050 (474 B): best 99.3% exact (regs 99.3%, ops 99.3%), 151/151 ins. Raw g2s 46.4%.
   This file also holds the three register-convention callees as statics defined before the caller (pilot
   finding 1): 0x0041f8f0 (record in ESI, 100%, byte-identical), 0x0041faf0 (ESI + cam, 90.8%), 0x0041fcc0
   (EDI + cam, 98.9%). With them static here the caller's ESI/EDI handling (lazy push edi / pop edi around the
   0x0041faf0..0x004c1a00 range) comes out as the original.
   Moved it: record array DAT_0063d9e0 of 0x2ac-byte CarRec; FUN_00426060 takes no argument and FUN_004e4320 /
   FUN_004e4350 take (world, cam); the light colour at DAT_0063d850 + 0x18 copied as a 4-float struct and scaled
   into a second one by +0x284; the two flag/mode blocks as if / else-if / goto skip.
   Residual (a): one x87 scheduling delta: the original issues the first fmul [esi+0x284] right after its fld,
   ours one mov later. Tried 3 spellings of the colour block; permuter 176 compiles: nothing. */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
typedef struct { float x, y, z; } V3;
typedef struct { V3 c; float r; } Sphere;
typedef struct { unsigned char r, g, b, a; } RGBA;
typedef struct { float r, g, b, a; } RGBAf;
typedef struct { int r, g, b, a; } Col4;
typedef struct { V3 right; unsigned int flags; V3 up; unsigned int pad1; V3 at; unsigned int pad2; V3 pos; unsigned int pad3; } RwMatrix;
typedef struct { char u00[0x10]; RwMatrix m; } RwFrame;
typedef int (*RsFn)(int, int);
typedef void (*DrawFn)(char *);
typedef struct {
  char *obj[0x3b];               /* +0x000, also indexed by +0x28c */
  char *atomic;                  /* +0x0ec */
  char u0f0[0x230 - 0xf0];
  float f230[6];                 /* +0x230 */
  Sphere s248;                   /* +0x248 */
  Sphere pos;                    /* +0x258 */
  char *veh;                     /* +0x268 */
  char *clump;                   /* +0x26c */
  char u270[0x284 - 0x270];
  float avg;                     /* +0x284 */
  float x288;                    /* +0x288 */
  int idx;                       /* +0x28c */
  unsigned int x290;             /* +0x290 */
  unsigned int x294;             /* +0x294 */
  int mode;                      /* +0x298 */
  char *cam0;                    /* +0x29c */
  char *cam;                     /* +0x2a0 */
  char *frame;                   /* +0x2a4 */
  int col;                       /* +0x2a8 */
} CarRec;
MGLOBAL(char *, DAT_0063d850, 0x0063d850);
MGLOBALA(int, DAT_0063d854, 2, 0x0063d854);
MGLOBALA(char *, DAT_0063e490, 2, 0x0063e490);
MGLOBALA(int, DAT_006146fc, 3, 0x006146fc);
MGLOBAL(char **, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(char *, DAT_0063e49c, 0x0063e49c);
MGLOBAL(char *, DAT_0063e4a0, 0x0063e4a0);
MGLOBALA(V3, DAT_005f5fe8, 13, 0x005f5fe8);
MGLOBALA(V3, DAT_005f6088, 13, 0x005f6088);
MGLOBAL(int, DAT_00636acc, 0x00636acc);
MGLOBALA(CarRec, DAT_0063d9e0, 4, 0x0063d9e0);

int FUN_004c1340(char *frame, V3 *v, int op);
void FUN_004b42c0(char *frame, V3 *a, int *b);
int FUN_004c1680(char *frame);
void FUN_00422ac0(unsigned int key, Col4 *c);
void FUN_00422af0(unsigned int key, char *cam);
int FUN_004c1bb0(char *cam, RGBA *c, int flags);
char *FUN_00426060(void);
void FUN_004e4320(char *world, char *cam);
int FUN_004c1a00(char *cam);
float FUN_00426e00(void);
int FUN_00426070(void);
void FUN_00422570(unsigned int key, char *m, int a, float alpha);
void FUN_004c19f0(char *cam);
void FUN_004e4350(char *world, char *cam);
char *FUN_004c0ed0(char *frame);
RwMatrix *FUN_004c52f0(RwMatrix *dst, void *src, int op);
RwMatrix *FUN_004c4d20(RwMatrix *m, V3 *axis, float angle, int op);
RwMatrix *FUN_004c51a0(RwMatrix *m, V3 *t, int op);
RwMatrix *FUN_004c5010(RwMatrix *m, V3 *s, int op);
RwMatrix *FUN_004c45f0(RwMatrix *m);
void FUN_004769f0(RGBA *c);
void FUN_00476a10(RwMatrix *m);
void FUN_00476d00(char *t);
int FUN_004c1b40(char *cam, Sphere *s);
int FUN_004260c0(void);
void FUN_004e4900(char *light, RGBAf *c);
void FUN_004e6680(char *clump);
void FUN_004228f0(int idx, char *cam);

// MATCH: 0x0041f8f0 FUN_0041f8f0
static void FUN_0041f8f0(CarRec *r)
{
  char *o = r->obj[r->idx];
  V3 *src;
  int i;
  RGBA bg;
  V3 p;
  Col4 c;

  if (DAT_0063d850 != 0 && o != 0) {
    p = r->pos.c;
    bg.r = 0;
    bg.g = 0;
    bg.b = 0;
    bg.a = 0;
    src = (V3 *)(*(char **)(DAT_0063d850 + 4) + 0x30);
    i = 0;
    if (r->mode == 2)
      i = 1;
    r->cam = DAT_0063e490[i];
    r->col = DAT_0063d854[i];
    r->frame = *(char **)(r->cam + 4);
    p.x += (float)(src->x * -4.0f);
    p.y += (float)(src->y * -4.0f);
    p.z += (float)(src->z * -4.0f);
    FUN_004c1340(r->frame, &p, 0);
    FUN_004b42c0(r->frame, &r->pos.c, DAT_006146fc);
    FUN_004c1680(r->frame);
    c.r = r->col;
    c.g = r->col;
    c.b = r->col;
    c.a = r->col;
    FUN_00422ac0(r->x290, &c);
    FUN_00422af0(r->x290, r->cam);
    FUN_004c1bb0(r->cam, &bg, 1);
    FUN_004e4320(FUN_00426060(), r->cam);
    if (FUN_004c1a00(r->cam)) {
      FUN_00422570(r->x290, *(char **)(r->clump + 4) + 0x10, FUN_00426070(), FUN_00426e00() * r->x288 * r->avg);
      ((RsFn)DAT_007d3ff8[8])(8, 0);
      ((RsFn)DAT_007d3ff8[8])(6, 0);
      (*(DrawFn *)(o + 0x48))(o);
      ((RsFn)DAT_007d3ff8[8])(6, 1);
      ((RsFn)DAT_007d3ff8[8])(8, 1);
      FUN_004c19f0(r->cam);
    }
    FUN_004e4350(FUN_00426060(), r->cam);
  }
}

// MATCH: 0x0041faf0 FUN_0041faf0
static void FUN_0041faf0(CarRec *r, char *cam)
{
  RGBA col;
  V3 d;
  V3 s;
  RwMatrix b;
  V3 axis;
  RwMatrix a;
  float dot;
  RwFrame *fr;

  col.r = 0xff;
  col.g = 0xff;
  col.b = 0xff;
  col.a = 0xff;
  axis.x = 1.0f;
  axis.y = 0.0f;
  axis.z = 0.0f;
  s.x = 0.75f;
  s.y = 2.0f;
  s.z = 1.0f;
  if (r->x294 & 0x40) {
    FUN_004c52f0(&a, FUN_004c0ed0(*(char **)(r->obj[0x19] + 4)), 0);
    FUN_004c52f0(&b, FUN_004c0ed0(*(char **)(r->obj[0x1c] + 4)), 0);
    d.x = a.pos.x - b.pos.x;
    d.y = a.pos.y - b.pos.y;
    d.z = a.pos.z - b.pos.z;
    FUN_004c4d20(&b, &axis, 90.0f, 1);
    b.pos.x += b.up.x * (s.y * 0.5f);
    b.pos.y += (s.y * 0.5f) * b.up.y;
    b.pos.z += (s.y * 0.5f) * b.up.z;
    b.pos.x += d.x * 0.5f;
    b.pos.y += d.y * 0.5f;
    b.pos.z += d.z * 0.5f;
    FUN_004c5010(&b, &s, 1);
    FUN_004c45f0(&b);
    fr = *(RwFrame **)(cam + 4);
    dot = fr->m.at.x * b.at.x + fr->m.at.y * b.at.y + fr->m.at.z * b.at.z;
    if (0.0f >= dot)
      dot = 0.0f;
    col.a = (unsigned char)(dot * 32.0f);
    FUN_004769f0(&col);
    FUN_00476a10(&b);
    FUN_00476d00(DAT_0063e49c);
  }
}

// MATCH: 0x0041fcc0 FUN_0041fcc0
static void FUN_0041fcc0(CarRec *r, char *cam)
{
  RGBA col;
  V3 axis;
  V3 t;
  V3 s;
  RwMatrix m;
  RwMatrix *fm;
  int i;
  float d;

  col.r = 0;
  col.g = 0;
  col.b = 0;
  col.a = 0xff;
  axis.x = 1.0f;
  axis.y = 0.0f;
  axis.z = 0.0f;
  i = *(int *)(r->veh + 0x80) / 6;
  s = DAT_005f5fe8[i];
  t = DAT_005f6088[i];
  fm = &(*(RwFrame **)(r->clump + 4))->m;
  FUN_004c52f0(&m, fm, 0);
  FUN_004c4d20(&m, &axis, 90.0f, 1);
  FUN_004c51a0(&m, &t, 1);
  FUN_004c5010(&m, &s, 1);
  FUN_004c45f0(&m);
  d = (fm->up.x * 0.0f + fm->up.y) + fm->up.z * 0.0f;
  if (0.0f >= d)
    d = 0.0f;
  col.a = (unsigned char)(d * r->x288 * 255.0f);
  FUN_004769f0(&col);
  FUN_00476a10(&m);
  FUN_00476d00(DAT_0063e4a0);
}

// MATCH: 0x00420050 FUN_00420050
void FUN_00420050(int idx, char *cam)
{
  CarRec *r = &DAT_0063d9e0[idx];
  char *o;
  RGBAf sv;
  RGBAf sc;

  if (r->x294 & 8) {
    if (FUN_004c1b40(cam, &r->pos)) {
      r->cam0 = cam;
      FUN_004c19f0(cam);
      FUN_004e4350(FUN_00426060(), cam);
      if (DAT_00636acc == 0 && FUN_004260c0())
        FUN_0041f8f0(r);
      FUN_0041faf0(r, cam);
      FUN_0041fcc0(r, cam);
      FUN_004e4320(FUN_00426060(), cam);
      FUN_004c1a00(cam);
      if (DAT_0063d850) {
        sv = *(RGBAf *)(DAT_0063d850 + 0x18);
        sc.r = sv.r * r->avg;
        sc.g = sv.g * r->avg;
        sc.b = sv.b * r->avg;
        sc.a = sv.a * r->avg;
        FUN_004e4900(DAT_0063d850, &sc);
      }
      FUN_004e6680(r->clump);
      if (!(r->x294 & 0x400)) {
        if (!(r->x294 & 0x10) && !(r->x294 & 0x1000)) {
          if (r->mode == 0)
            o = r->obj[0xe];
          else if (r->mode == 1)
            o = r->obj[0x2f];
          else
            goto skip1;
          if (o)
            (*(DrawFn *)(o + 0x48))(o);
        }
skip1:
        if (!(r->x294 & 0x20) && !(r->x294 & 0x2000)) {
          if (r->mode == 0)
            o = r->obj[0xf];
          else if (r->mode == 1)
            o = r->obj[0x30];
          else
            goto skip2;
          if (o)
            (*(DrawFn *)(o + 0x48))(o);
        }
      }
skip2:
      if (DAT_0063d850)
        FUN_004e4900(DAT_0063d850, &sv);
      if (DAT_00636acc == 0 && FUN_004260c0()) {
        ((RsFn)DAT_007d3ff8[8])(8, 0);
        FUN_004228f0(idx, cam);
        ((RsFn)DAT_007d3ff8[8])(8, 1);
      }
    }
  }
}
