// FLAGS: /O2 /GS /Ob1
/* 0x0047ba20 (box edge test: d = normalize(pt - p[e[0]]), d1/d2 = dot with n[e[2]] / n[e[3]], optional
   *side = d1 <= d2, returns d1*d2 < DAT_005d757c): best 60.8% (regs 71.6%), raw 37.8%.
   Done: layout float n[12][3] @0, float p[8][3] @0x90 (the x read via lea idx*3+0x24 and y/z via +0x94/+0x98
   now match), uchar e[][4] @0xf0, ternary for *side (jne/xor/jmp shape). Tried: RwV3d structs vs float
   arrays, __inline Dot helper, every term/factor order of the dot products (all compile identically: 9176
   puts the z term first, the original y, x, z), five forms of the side store, permuter 2x300 s (its only
   gains moved n1's assignment after its use, rejected). Remaining: pt->x loaded before p.x (original
   fld p.x; fsubr [pt]), the dot-product term order and d1/d2 interleave. Class (b). */
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
typedef struct BoxShape {
  float n[12][3];         /* +0x00 */
  float p[8][3];          /* +0x90 */
  unsigned char e[12][4]; /* +0xf0 */
} BoxShape;
MGLOBAL(float, DAT_005d757c, 0x005d757c);
float FUN_004c39b0(float *out, const float *in);
// MATCH: 0x0047ba20 FUN_0047ba20
int FUN_0047ba20(BoxShape *b, int e, float *pt, int *side)
{
  float d[3];
  float *n1;
  float *n2;
  float d1;
  float d2;
  int i;
  i = b->e[e][0];
  d[0] = pt[0] - b->p[i][0];
  d[1] = pt[1] - b->p[i][1];
  d[2] = pt[2] - b->p[i][2];
  FUN_004c39b0(d, d);
  n1 = b->n[b->e[e][2]];
  d1 = n1[1] * d[1] + d[0] * n1[0] + n1[2] * d[2];
  n2 = b->n[b->e[e][3]];
  d2 = n2[1] * d[1] + d[0] * n2[0] + n2[2] * d[2];
  if (side != 0) {
    *side = d1 > d2 ? 0 : 1;
  }
  if (d1 * d2 < DAT_005d757c) {
    return 1;
  }
  return 0;
}
