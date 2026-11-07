// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d9ee0 (h3): best 82.8% (regs 82.8%, ops 100%). Raw 38.7%. A 4-float struct for the two scaled vectors
   (ma -= mb in place) reproduces the spill pattern and every op. Residual: the 8 products, original
   'fld [mem]; fmul st(i)' against ours 'fld st(i); fmul [mem]'. Tried both operand orders, struct copy then
   scale, load then scale. Class (a): x87 operand order in products (S19 flags brief list). */
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
typedef struct V4_9ee0 { float x, y, z, w; } V4_9ee0;
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
// MATCH: 0x004d9ee0 FUN_004d9ee0
void FUN_004d9ee0(float *param_1, float *param_2, float *param_3)
{
  float sum;
  float ia, ib;
  sum = param_2[5] + param_3[5];
  param_1[5] = sum;
  if (param_2[0] > DAT_005d757c && param_3[0] > DAT_005d757c) {
    ia = DAT_005cc320 / param_2[0];
    ib = DAT_005cc320 / param_3[0];
    {
      V4_9ee0 ma, mb;
      ma.x = param_2[1] * ia; ma.y = param_2[2] * ia; ma.z = param_2[3] * ia; ma.w = param_2[4] * ia;
      mb.x = param_3[1] * ib; mb.y = param_3[2] * ib; mb.z = param_3[3] * ib; mb.w = param_3[4] * ib;
      ma.x -= mb.x; ma.y -= mb.y; ma.z -= mb.z; ma.w -= mb.w;
      param_1[5] = (ma.x * ma.x + ma.y * ma.y + ma.z * ma.z + ma.w * ma.w) / (ia + ib) + sum;
    }
  }
  param_1[1] = param_2[1] + param_3[1];
  param_1[2] = param_2[2] + param_3[2];
  param_1[3] = param_2[3] + param_3[3];
  param_1[4] = param_2[4] + param_3[4];
  param_1[0] = param_2[0] + param_3[0];
}
