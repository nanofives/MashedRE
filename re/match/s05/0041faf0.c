// FLAGS: /O2 /GS /Ob1
/* 0x0041faf0 FUN_0041faf0 (458 B): best 90.8% exact (regs 90.8%, ops 93.3%), 119/119 ins. Raw g2s 23.5%.
   The record arrives in ESI and cam on the stack: the real caller FUN_00420050 pushes ebx (cam) at 0x004200c7
   with the record in ESI (the push edi before it saves EDI for the next call, it is not an argument).
   Static-function convention (pilot finding 1); Caller_0041faf0 is a stand-in. FUN_00420050's file
   (00420050.c) holds this function too, next to its real caller, at the same score.
   Moved it: locals laid out as the frame (RGBA col @0, d @4, scale @0x10, matrix b @0x1c, axis @0x5c, matrix a
   @0x68), FUN_004c0ed0 takes 1 argument (the push 0 is FUN_004c52f0's third), the (s.y * 0.5f) common
   subexpression written out three times instead of a named temp (a temp kept an extra x87 slot), 0.0 as the
   literal 0.0f (0x005d757c is the pooled __real@00000000) and the clamp spelled 0.0f >= dot (test ah,1 / jne).
   Residual (b): (1) the three-term dot of the camera frame's at vector with b.at: ours loads b.at first and in
   z,y,x order, the original loads the camera side first in y,x,z order. All 12 term/operand orders give
   identical code (cl canonicalises the sum); pointer temps, an __inline dot helper, float-array access and
   split += statements did not move it. (2) two x87 scheduling deltas in the d.y subtraction and the
   b.pos += up * f block. Permuter 299 compiles: nothing. */
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
  char u000[0x64];
  char *objA;                    /* +0x064 */
  char u068[0x70 - 0x68];
  char *objB;                    /* +0x070 */
  char u074[0x294 - 0x74];
  unsigned int x294;             /* +0x294 */
} CarRec;
MGLOBAL(char *, DAT_0063e49c, 0x0063e49c);

char *FUN_004c0ed0(char *frame);
RwMatrix *FUN_004c52f0(RwMatrix *dst, char *src, int op);
RwMatrix *FUN_004c4d20(RwMatrix *m, V3 *axis, float angle, int op);
RwMatrix *FUN_004c5010(RwMatrix *m, V3 *s, int op);
RwMatrix *FUN_004c45f0(RwMatrix *m);
void FUN_004769f0(RGBA *c);
void FUN_00476a10(RwMatrix *m);
void FUN_00476d00(char *t);

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
    FUN_004c52f0(&a, FUN_004c0ed0(*(char **)(r->objA + 4)), 0);
    FUN_004c52f0(&b, FUN_004c0ed0(*(char **)(r->objB + 4)), 0);
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

/* stand-in for the real caller FUN_00420050 (call at 0x004200c8, record in ESI, cam pushed); not matched */
void Caller_0041faf0(CarRec *r, char *cam)
{
  FUN_0041faf0(r, cam);
  FUN_0041faf0(r, cam);
}
