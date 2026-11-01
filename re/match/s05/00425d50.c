// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00425d50 (236 B), h9 by hand: 100.0% (regs 100.0%). Raw 21.6%. 8-entry table of 0x4c-byte records at
   0x00899260 (DAT_008992a4 = record 0 +0x44) declared as one MGLOBALA struct array; param_1 is unsigned (the
   fadd 2^32 fix-up); the four .rdata floats are literals (2^32 is the compiler's own, 1/3000, 0.05, -0.05).
   What moved it: an int index loop (a pointer loop became a dec-counter), and updating e->time in place
   (a float temp stayed in st0 across the branch). */
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
typedef struct { float x; float y; float z; } V3;
typedef struct { V3 right; int flags; V3 up; int pad1; V3 at; int pad2; V3 pos; int pad3; } RwMatrix;
typedef struct { int obj[2]; int link[2]; RwMatrix modelling; } RwFrame;
typedef struct { int a; RwFrame *frame; } Obj;
typedef struct { int pad[3]; float duration; } Anim;
typedef struct {
  Obj *obj;        /* +0x00 */
  int pad04;       /* +0x04 */
  Obj *part0;      /* +0x08 */
  Obj *part1;      /* +0x0c */
  int pad10[9];    /* +0x10 */
  V3 pos;          /* +0x34 */
  float time;      /* +0x40 */
  Anim *anim;      /* +0x44 */
  int pad48;       /* +0x48 */
} AnimObj;
MGLOBALA(AnimObj, DAT_00899260, 8, 0x00899260);
void FUN_00425ca0(Obj *p0, float p1);
void FUN_004752f0(RwMatrix *p0, Anim *p1, float p2);
RwFrame *FUN_004c1480(RwFrame *p0, RwMatrix *p1, int p2);
// MATCH: 0x00425d50 FUN_00425d50
void FUN_00425d50(unsigned int param_1)
{
  float dt;
  float step;
  RwFrame *f;
  AnimObj *e;
  int i;
  RwMatrix m;

  dt = (float)param_1;
  step = dt * (1.0f / 3000.0f);
  for (i = 0; i < 8; i++) {
    e = &DAT_00899260[i];
    if (e->obj != 0 && e->anim != 0) {
      f = e->obj->frame;
      e->time = step + e->time;
      if (e->time > e->anim->duration) {
        e->time = e->time - e->anim->duration;
      }
      FUN_004752f0(&m, e->anim, e->time);
      FUN_004c1480(f, &m, 0);
      e->pos = m.pos;
      FUN_00425ca0(e->part0, dt * 0.05f);
      FUN_00425ca0(e->part1, dt * -0.05f);
    }
  }
}
