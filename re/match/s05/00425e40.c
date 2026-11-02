// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00425e40 (86 B), h9 by hand: 100.0% (regs 100.0%). Raw 52.8%. Same 8 x 0x4c table as 0x00425d50
   (MGLOBALA at 0x00899260), int index loop; atomics typed with the render callback at +0x48; callees renamed
   (RpAtomicGetWorldBoundingSphere -> FUN_004e6100, 1 arg). DAT_007d3ff8 is RwGlobals*, +0 = current camera. */
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
typedef struct Atomic Atomic;
typedef Atomic *(*AtomicRenderCB)(Atomic *a);
struct Atomic {
  int object;               /* +0x00 */
  void *frame;              /* +0x04 */
  int pad08[16];            /* +0x08 */
  AtomicRenderCB render;    /* +0x48 */
};
typedef struct {
  void *clump;     /* +0x00 */
  Atomic *body;    /* +0x04 */
  Atomic *part0;   /* +0x08 */
  Atomic *part1;   /* +0x0c */
  int pad10[12];   /* +0x10 */
  float time;      /* +0x40 */
  void *anim;      /* +0x44 */
  int pad48;       /* +0x48 */
} AnimObj;
typedef struct { void *curCamera; } RwGlobalsHead;
MGLOBALA(AnimObj, DAT_00899260, 8, 0x00899260);
MGLOBAL(RwGlobalsHead *, DAT_007d3ff8, 0x007d3ff8);
int FUN_004c1b40(void *camera, void *sphere);
void *FUN_004e6100(Atomic *a);
// MATCH: 0x00425e40 FUN_00425e40
void FUN_00425e40(void)
{
  int i;
  AnimObj *e;

  for (i = 0; i < 8; i++) {
    e = &DAT_00899260[i];
    if (e->anim != 0 && e->clump != 0) {
      if (FUN_004c1b40(DAT_007d3ff8->curCamera, FUN_004e6100(e->body)) != 0) {
        e->body->render(e->body);
        e->part1->render(e->part1);
        e->part0->render(e->part0);
      }
    }
  }
}
