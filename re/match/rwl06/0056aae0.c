// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x0056aae0: hand-matched (h2). 100.0% regs 100.0%. Raw form had Ghidra in_stack_* floats. Real signature: 14 dword params, then a 0x88-byte struct by value (34 dwords, forwarded to 0x0056a7a0 by value), then 7 float params (a..g at entry+0xc4..0xdc). Param 3..5 is a 12-byte struct by value ({int *base, w, w}), forwarded whole to 0x0056a7a0 (that makes the sub esp,0xc / mov [eax] copy). Callee 0x0056a7a0 prototype: (V3 v, S s, float t, int idx). */
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
typedef unsigned int undefined4;
typedef struct { int d[34]; } S0056aae0;
typedef struct { int a; undefined4 b; undefined4 c; } V0056aae0;
void FUN_0056a7a0(V0056aae0 v, S0056aae0 s, float t, int idx);
// MATCH: 0x0056aae0 FUN_0056aae0
void FUN_0056aae0(int *param_1,int *param_2,V0056aae0 param_3,
                 int param_6,undefined4 param_7,undefined4 param_8,int param_9,undefined4 param_10,
                 undefined4 param_11,int param_12,undefined4 param_13,undefined4 param_14,
                 S0056aae0 s,float a,float b,float c,float d,float e,float f,float g)
{
  float t;

  t = d - a;
  if (t < b) {
    t = b;
  }
  if (c < t) {
    t = c;
  }
  *(float *)(param_12 + (*param_2 + *param_1 * 4) * 4) = t;
  if (t == b) {
    *(undefined4 *)(param_6 + (*param_2 + *param_1 * 4) * 4) = 0;
  }
  else {
    *(undefined4 *)(param_6 + (*param_2 + *param_1 * 4) * 4) = 0xffffffff;
  }
  if (t == c) {
    *(undefined4 *)(param_9 + (*param_2 + *param_1 * 4) * 4) = 0;
  }
  else {
    *(undefined4 *)(param_9 + (*param_2 + *param_1 * 4) * 4) = 0xffffffff;
  }
  t = t - d;
  FUN_0056a7a0(param_3,s,t,*param_2 + *param_1 * 4);
  *(float *)(param_3.a + (*param_2 + *param_1 * 4) * 4) =
       (e * g + f) * t + *(float *)(param_3.a + (*param_2 + *param_1 * 4) * 4);
}
