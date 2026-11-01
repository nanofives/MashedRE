// FLAGS: /O2 /GS /Ob1
/* 0x0041fcc0 FUN_0041fcc0 (329 B): best 98.9% exact (regs 98.9%, ops 98.9%), 92/92 ins. Raw g2s 21.7%.
   The record arrives in EDI and cam on the stack (unused by the body): the real caller FUN_00420050 does
   push ebx; mov edi,esi at 0x004200cd. Static-function convention (pilot finding 1); Caller_0041fcc0 is a
   stand-in. FUN_00420050's file (00420050.c) holds this function too, at the same score.
   Moved it: locals laid out as the frame (RGBA @0, axis @4, translate @0x10, scale @0x1c, matrix @0x28); the
   tables at 0x005f5fe8 / 0x005f6088 as V3 arrays indexed by veh+0x80 / 6 (struct copies); 0.0 as the literal
   0.0f (0x005d757c is the pooled __real@00000000: with an MGLOBAL float cl factors x*k + z*k into (x+z)*k and
   loads k first); the sum parenthesised (up.x * 0.0f + up.y) + up.z * 0.0f; clamp 0.0f >= d; 255.0 literal.
   Residual (a): one instruction, pop esi scheduled one x87 load later in the original (after the fld 0.0 of the
   clamp). Tried 6 sum spellings, 5 clamp spellings, ternary; permuter 241 compiles: nothing. */
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
typedef struct { V3 right; unsigned int flags; V3 up; unsigned int pad1; V3 at; unsigned int pad2; V3 pos; unsigned int pad3; } RwMatrix;
typedef struct { char u00[0x10]; RwMatrix m; } RwFrame;
typedef struct {
  char u000[0x268];
  char *veh;                     /* +0x268 */
  char *clump;                   /* +0x26c */
  char u270[0x288 - 0x270];
  float x288;                    /* +0x288 */
} CarRec;
MGLOBALA(V3, DAT_005f5fe8, 13, 0x005f5fe8);
MGLOBALA(V3, DAT_005f6088, 13, 0x005f6088);
MGLOBAL(char *, DAT_0063e4a0, 0x0063e4a0);

RwMatrix *FUN_004c52f0(RwMatrix *dst, RwMatrix *src, int op);
RwMatrix *FUN_004c4d20(RwMatrix *m, V3 *axis, float angle, int op);
RwMatrix *FUN_004c51a0(RwMatrix *m, V3 *t, int op);
RwMatrix *FUN_004c5010(RwMatrix *m, V3 *s, int op);
RwMatrix *FUN_004c45f0(RwMatrix *m);
void FUN_004769f0(RGBA *c);
void FUN_00476a10(RwMatrix *m);
void FUN_00476d00(char *t);

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

/* stand-in for the real caller FUN_00420050 (call at 0x004200d0: record in EDI, cam pushed); not matched */
void Caller_0041fcc0(CarRec *r, char *cam)
{
  FUN_0041fcc0(r, cam);
  FUN_0041fcc0(r, cam);
}
