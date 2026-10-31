// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041f590 by hand (h9): best 78.4% (regs 78.4%, ops 83.5%), 97/96 ins; natural statement order 70.8%, the permuter's file moves line.end.z = pos->z up (local-only store, same semantics). Raw g2s 32.1%. The record arrives in ESI (Ghidra unaff_ESI): FUN_0041f590 is file-static and the file holds a stand-in caller (Caller_00420870, NOT matched; the real caller 0x00420870 passes the same record in EDI to 0x0041f4f0 and in ESI here). Moved it: record struct (hist[8] +0x200 shifted down by a 7-step loop, obj +0x26c, avg +0x284, +0x288, key +0x290), unrolled sum r->avg += hist[i], float literals (0.0625/0.9375/1.0/0.125/0.0; pooled at 0x005cd058/0x005cd278/0x005cc320/0x005cd050/0x005d757c), and the MIN-then-MAX macro shape ((1.0f <= f ? 1.0f : f) >= 0.0f ? (1.0f <= f ? 1.0f : f) : 0.0f), which gives the original's jnp / test ah,1 / jp branch layout (an extern const at 0x005d757c breaks the fold the original shows). Residuals: (1) the six line endpoint copies are fld/fstp in the original, mov in ours (tried float locals, struct copy + adjust, aggregate initializer, __inline V3Set, float* indexing, +0.0/*1.0/(double) casts); (2) the clamped value stays in st0 across the branches in the original, ours spills it to a stack slot (tried r->avg vs local f, chained assignment, 64 comparison spellings). Class (b). */
/* ---- MATCH-S04 preamble (same text in every re/match/s04 source; not part of any function body) ----
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
typedef struct { V3 start, end; } Line;
typedef struct {
  char u000[0x200];
  float hist[8];                 /* +0x200 */
  char u220[0x26c - 0x220];
  char *obj;                     /* +0x26c */
  char u270[0x284 - 0x270];
  float avg;                     /* +0x284 */
  float x288;                    /* +0x288 */
  int u28c;
  unsigned int x290;             /* +0x290 */
} CarRec;
MGLOBAL(unsigned int, DAT_007f101c, 0x007f101c);
char *FUN_004c0ed0(int frame);
int FUN_00426dc0(Line *line, float *hit, float *frac);
// MATCH: 0x0041f590 FUN_0041f590
static void FUN_0041f590(CarRec *r)
{
  float hit = 0.0f;
  float frac = 1.0f;
  Line line;
  int i;
  V3 *pos;
  float f;
  for (i = 7; i > 0; i--)
    r->hist[i] = r->hist[i - 1];
  pos = (V3 *)(FUN_004c0ed0(*(int *)(r->obj + 4)) + 0x30);
  line.start.x = pos->x;
  line.end.z = pos->z;
  line.start.y = pos->y + 0.0625f;
  line.start.z = pos->z;
  line.end.x = pos->x;
  line.end.y = pos->y - 0.9375f;
  if ((DAT_007f101c & 3) == r->x290) {
    if (FUN_00426dc0(&line, &hit, &frac))
      r->hist[0] = hit;
    r->x288 = 1.0f - frac;
  }
  r->avg = 0.0f;
  for (i = 0; i < 8; i++)
    r->avg += r->hist[i];
  r->avg *= 0.125f;
  f = r->avg;
  r->avg = ((1.0f <= f ? 1.0f : f) >= 0.0f ? (1.0f <= f ? 1.0f : f) : 0.0f);
}
/* stand-in caller shaped like the call site in 0x00420870 (not matched here) */
void Caller_00420870(CarRec *r, int a)
{
  FUN_0041f590(r);
  FUN_0041f590(r);
}
