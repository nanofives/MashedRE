// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 h3 0x0056fad0: best 76.5% (regs 76.5%, ops 98.5%), class (a). V3 struct copy found; the fmul/fld operand order of every product is canonicalised by the compiler (source flips change nothing); permuter no gain. */
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
typedef unsigned char undefined1;
MGLOBAL(undefined1, DAT_005cc33c, 0x005cc33c);
MGLOBAL(float, _DAT_005cc33c, 0x005cc33c);
typedef struct V3 { float x, y, z; } V3;
// MATCH: 0x0056fad0 FUN_0056fad0
/* [C1 2026-05-19] Signature: `void FUN_0056fad0(float *param_1, float *param_2, float *param_3,
   float *param_4, int param_5, int
 */

void FUN_0056fad0(float *param_1,float *param_2,float *param_3,float *param_4,int param_5,
                 int param_6)

{
  if (param_5 != 0) {
    *(V3 *)param_1 = *(V3 *)param_2;
    param_1[4] = param_2[2] * param_3[1] - param_2[1] * param_3[2];
    param_1[5] = param_3[2] * *param_2 - param_2[2] * *param_3;
    param_1[6] = param_2[1] * *param_3 - param_3[1] * *param_2;
  }
  if (param_6 != 0) {
    param_1[8] = *param_2 * _DAT_005cc33c;
    param_1[9] = param_2[1] * _DAT_005cc33c;
    param_1[10] = param_2[2] * _DAT_005cc33c;
    param_1[0xc] = param_2[1] * param_4[2] - param_4[1] * param_2[2];
    param_1[0xd] = param_2[2] * *param_4 - param_4[2] * *param_2;
    param_1[0xe] = param_4[1] * *param_2 - param_2[1] * *param_4;
  }
  return;
}
