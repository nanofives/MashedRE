// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x004818d0 (h10): 98.9% (regs 98.9%). Moved it: 0x005cc990 / 0x005cd0ec are float literals
   1e-05f / 0.005f; FUN_0047d100 is called with 2 pushed args (push 0; push esi) although its body
   (re/match/s13/0047d100.c) reads one, so it is declared (int, int) here; FUN_00453eb0 takes float args;
   sum order z*z + x*x + y*y. Raw 56.4%. Residual (b): one store (mov [esp+0x10],ecx of v.z) is scheduled
   after the first fld instead of before it. Tried: all 6 sum orders, (a+b)+c groupings, field-by-field copy,
   a pointer local, float[3], an __inline lensq helper, double d, folding d into the if, permuter (189
   compiles, no gain). */
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
typedef struct V3 { float x, y, z; } V3;
typedef struct V4 { float x, y, z, w; } V4;
MGLOBAL(V3, DAT_006146fc, 0x006146fc);
MGLOBALA(float, DAT_006c6870, 200, 0x006c6870);
MGLOBALA(int, DAT_006c6b90, 200, 0x006c6b90);
MGLOBALA(int, DAT_006c71d8, 200, 0x006c71d8);
MGLOBALA(unsigned int, DAT_006c9438, 200, 0x006c9438);
void FUN_00453eb0(V4 *p0, float p1, float p2, float p3, int p4);
void FUN_00476860(V3 *p0, V3 *p1, float p2);
void FUN_0047d080(int p0, int p1);
void FUN_0047d100(int p0, int p1);
void FUN_0048a4e0(int p0, V3 *p1);
int FUN_0057c210(int p0);
// MATCH: 0x004818d0 FUN_004818d0
void FUN_004818d0(int param_1)
{
  int h;
  int o;
  int *g;
  V3 *pos;
  float d;
  V3 v;
  V4 q;

  h = DAT_006c71d8[param_1];
  o = FUN_0057c210(h);
  if (o != 0 && (g = *(int **)(*(int *)(o + 0x10) + 4)) != 0) {
    v = *(V3 *)(*(int *)(*(int *)(*g + 0x10) + 8) + g[1] * 0x20);
    { float tF_ = v.x; d = v.z * v.z + tF_ * v.x + v.y * v.y; }
    if (d > 1e-05f) {
      DAT_006c9438[param_1] |= 8;
    }
    if (d > DAT_006c6870[param_1]) {
      v.x *= 0.005f;
      v.y *= 0.005f;
      v.z *= 0.005f;
      FUN_0048a4e0(h, &v);
      FUN_0047d080(param_1, 0);
      FUN_0047d100(param_1, 0);
      DAT_006c9438[param_1] |= 2;
      if (DAT_006c6b90[param_1] >= 150000) {
        pos = (V3 *)(*(int *)(h + 4) + 0x40);
        FUN_00476860(pos, &DAT_006146fc, 360.0f);
        q.x = pos->x;
        q.y = pos->y;
        q.z = pos->z;
        q.w = 1.0f;
        FUN_00453eb0(&q, 30.0f, 400.0f, 100.0f, 1);
      }
    }
  }
}
