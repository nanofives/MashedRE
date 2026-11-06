// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x004829d0 (h9, by hand): 100% (regs 100%). Replay frame append; 0x24-byte frame record
   {Q4 q; V3 p; next; t}; the two copies to the new frame are struct assignments. Residual: none. */
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
typedef void *(*AllocFn)(int size, int flags);
typedef struct V3 { float x, y, z; } V3;
typedef struct Q4 { float x, y, z, w; } Q4;
typedef struct RFrame {
  Q4 q;                 /* +0x00 */
  V3 p;                 /* +0x10 */
  struct RFrame *next;  /* +0x1c */
  int t;                /* +0x20 */
} RFrame;
typedef struct RMat { float m[12]; V3 pos; } RMat;
typedef struct Replay {
  int magic;            /* +0x000 */
  int pad04[6];
  RFrame *cur;          /* +0x01c */
  int pad20[0x52];
  int count;            /* +0x168 */
  int n16c;             /* +0x16c */
  int n170;             /* +0x170 */
  int t174;             /* +0x174 */
  int t178;             /* +0x178 */
  int pad17c[6];
  int started;          /* +0x194 */
} Replay;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
int FUN_00546b10(RFrame *p0, RMat *p1);
// MATCH: 0x004829d0 FUN_004829d0
void FUN_004829d0(Replay *r, RMat *m, int t, int n)
{
  if (r->n170 > n || r->started == 0) {
    r->cur->p.x = m->pos.x;
    r->cur->p.y = m->pos.y;
    r->cur->p.z = m->pos.z;
    FUN_00546b10(r->cur, m);
    r->cur->t = t;
    if (r->cur->next == 0) {
      int i;
      int *w;
      r->cur->next = (RFrame *)(*(AllocFn *)(DAT_007d3ff8 + 0x108))(0x24, 0);
      w = (int *)r->cur->next;
      for (i = 0; i < 9; i++) {
        w[i] = 0;
      }
    }
    r->cur->next->p = r->cur->p;
    r->cur->next->q = r->cur->q;
    r->cur = r->cur->next;
    r->t174 = t;
    r->count++;
    r->n170 = 1;
    if (r->started == 0) {
      r->t178 = t;
    }
    r->started = 1;
  }
  r->n170++;
}
