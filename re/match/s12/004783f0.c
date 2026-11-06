// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x004783f0 (h4, by hand + permuter): 90.8% (regs 90.8%). Struct Grp (dist[0x1800] float at +0, ang[0x1800] at +0x6000, 0x4c-byte nodes at +0xc1c0, count at +0x16244); two-statement dot with a local axis {1,0,0} (one expression lets the compiler factor the zero products); v[2]=0 placed before the distance (permuter) moves the node-pointer leas. Residual (b): the two node-base leas sit after the first fld in the original, after the third fsub here; the loop-pad jmp follows from that size shift. Epilogue past Ghidra size. */
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
int FUN_00409300(int p0);
int FUN_00409330(int p0, int p1);
double FUN_004a3384(double x);
float FUN_004c39b0(float *out, float *in);
float FUN_004c3b30(float x);

typedef struct Node4c {
  float d0;
  float d4;
  float d8;
  float x;
  float y;
  float z;
  char pad[0x4c - 0x18];
} Node4c;

typedef struct Grp {
  float dist[0x1800];
  float ang[0x1800];
  char pad[0xc1c0 - 0xc000];
  Node4c node[0x21c];
  char pad2[0x16244 - 0xc1c0 - 0x21c * 0x4c];
  int count;
} Grp;

// MATCH: 0x004783f0 FUN_004783f0
void FUN_004783f0(Grp *g)
{
  int going;
  int cur;
  int next;
  int i;
  float v[3];
  float ax[3];
  float dx, dy, dz;
  float d;
  float ang;
  Node4c *a;
  Node4c *b;

  for (i = 0; i < g->count; i++) {
    going = 1;
    cur = FUN_00409300(i);
    do {
      next = FUN_00409330(cur, i);
      if (next == FUN_00409300(i)) {
        going = 0;
      }
      v[2] = 0.0f;
      dx = g->node[cur].x - g->node[next].x;
      dy = g->node[cur].y - g->node[next].y;
      dz = g->node[cur].z - g->node[next].z;
      g->dist[i * 0x200 + cur] = FUN_004c3b30(dx * dx + dy * dy + dz * dz);
      v[0] = g->node[cur].d0;
      v[1] = g->node[cur].d8;
      FUN_004c39b0(v, v);
      ax[0] = 1.0f;
      ax[1] = 0.0f;
      ax[2] = 0.0f;
      d = v[0] * ax[0] + v[1] * ax[1];
      d = d + v[2] * ax[2];
      if (d < -1.0) d = -1.0f;
      if (d > 1.0) d = 1.0f;
      ang = (float)FUN_004a3384(d) * 57.29578f;
      if (v[0] * ax[1] - v[1] * ax[0] < 0.0f) ang = -ang;
      while (ang <= 0.0f) ang += 360.0f;
      g->ang[i * 0x200 + cur] = ang;
      cur = next;
    } while (going);
  }
}
