// FLAGS: /O2 /GS /Ob1
/* 0x0041f8f0 FUN_0041f8f0 (500 B): 100.0% exact, regs 100.0%, whole symbol BYTE-IDENTICAL.
   Raw g2s was 49.7% (unaff_ESI). The record arrives in ESI: the caller FUN_00420050 holds it in ESI at the call
   0x004200c1. Static-function convention (pilot finding 1); Caller_0041f8f0 is a stand-in (not matched).
   What moved it: record struct (obj table indexed by +0x28c, pos +0x258, cam/frame/col +0x2a0..+0x2a8);
   i = 0; if (mode == 2) i = 1 (a branch, not sete); src = matrix + 0x30 as a V3 pointer; p.x += (float)(src->x *
   -4.0f) (without the cast cl folds the negative literal into fsubr with +4.0f; -4.0f is pooled at 0x005cca00);
   and the real callee arities (pilot finding 3): FUN_00426060 takes no argument (mov eax,[0x0065742c]; ret) and
   FUN_004e4320 / FUN_004e4350 take (world, cam), so r->cam is pushed before the inner call, which is what
   flushed the deferred add esp at the right place. RGBA bg {0,0,0,0} as a 4-byte struct, Col4 four ints.
   Residual: none. */
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
typedef struct { unsigned char r, g, b, a; } RGBA;
typedef struct { int r, g, b, a; } Col4;
typedef int (*RsFn)(int, int);
typedef void (*DrawFn)(char *);
typedef struct {
  char *obj[0x96];               /* +0x000, indexed by +0x28c */
  V3 pos;                        /* +0x258 */
  char u264[0x26c - 0x264];
  char *clump;                   /* +0x26c */
  char u270[0x284 - 0x270];
  float avg;                     /* +0x284 */
  float x288;                    /* +0x288 */
  int idx;                       /* +0x28c */
  unsigned int x290;             /* +0x290 */
  unsigned int x294;             /* +0x294 */
  int mode;                      /* +0x298 */
  int u29c;
  char *cam;                     /* +0x2a0 */
  char *frame;                   /* +0x2a4 */
  int col;                       /* +0x2a8 */
} CarRec;
MGLOBAL(char *, DAT_0063d850, 0x0063d850);
MGLOBALA(int, DAT_0063d854, 2, 0x0063d854);
MGLOBALA(char *, DAT_0063e490, 2, 0x0063e490);
MGLOBALA(int, DAT_006146fc, 3, 0x006146fc);
MGLOBAL(char **, DAT_007d3ff8, 0x007d3ff8);

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
int FUN_00426070(float f);
void FUN_00422570(unsigned int key, char *m, int a);
void FUN_004c19f0(char *cam);
void FUN_004e4350(char *world, char *cam);

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
    p = r->pos;
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
    FUN_004b42c0(r->frame, &r->pos, DAT_006146fc);
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
      FUN_00422570(r->x290, *(char **)(r->clump + 4) + 0x10, FUN_00426070(FUN_00426e00() * r->x288 * r->avg));
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

/* stand-in for the real caller FUN_00420050 (call at 0x004200c1 with the record in ESI); not matched */
void Caller_0041f8f0(CarRec *r, int a)
{
  FUN_0041f8f0(r);
  FUN_0041f8f0(r);
}
