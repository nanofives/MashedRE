// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 0x004858c0 (h2): 100% (raw 51.6%). Moved: grid struct; the four _DAT_005cc564/5cc328/5ccac8/
   5cc8f0 floats are .rdata literals (0.25f, 0.01f, 1.0f/3.0f = 0x3eaaaaab, 0.15f): a global operand is loaded
   first, a literal is the fmul operand (relocheck OK, unchecked=0); Vec3 struct copies max then min;
   the clear loops in Ghidra's pre-decrement pointer form (s = ps before a = pi). */
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
typedef struct Vec3 { float x, y, z; } Vec3;
typedef struct Aabb { Vec3 min; Vec3 max; } Aabb;
typedef struct Grid4858c0 {
  char f000[0x20];
  short cells[8][16];
  char f120[0x20];
  int items[8][16];
  char f340[0x40];
  int counts[8];
  int f3a0;
  char f3a4[0x7c];
  Vec3 v420;
  Vec3 v42c;
  Vec3 min;
  Vec3 max;
  char f450[0x18];
  float f468;
  float f46c;
  float f470;
  int f474;
  int f478;
} Grid4858c0;
void FUN_004854e0(Grid4858c0 *g, float f);
// MATCH: 0x004858c0 FUN_004858c0
Grid4858c0 *FUN_004858c0(Aabb *box, float scale, int param_3, Grid4858c0 *g)
{
  int *pc;
  int *pi;
  short *ps;
  int *a;
  short *s;
  int n;
  int k;
  g->f478 = param_3;
  g->max = box->max;
  g->min = box->min;
  FUN_004854e0(g, 0.5f);
  g->f470 = scale * 0.25f;
  if (g->f470 > 0.01f) {
    g->f470 = 0.01f;
  }
  g->f46c = scale * (1.0f / 3.0f);
  if (g->f46c > 0.15f) {
    g->f46c = 0.15f;
  }
  g->f468 = scale;
  g->f474 = 0;
  g->v420.z = 0;
  g->v420.y = 0;
  g->v420.x = 0;
  g->v42c.z = 0;
  g->v42c.y = 0;
  g->v42c.x = 0;
  pc = &g->counts[8];
  pi = &g->items[8][0];
  ps = &g->cells[8][0];
  n = 8;
  do {
    pc--;
    pi -= 16;
    *pc = 0;
    ps -= 16;
    g->f3a0 = 0;
    k = 16;
    s = ps;
    a = pi;
    do {
      a--;
      s--;
      k--;
      *a = 0;
      *s = -1;
    } while (k != 0);
  } while (--n != 0);
  return g;
}
