// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x0055c2d0: hand-matched (h3). 89.2% (regs 89.2, ops 98.5). float v[4] array keeps all four result stores (the 4 separate locals lost two); x2c2 + (x1c1 + x0c0) order. Residue: rows 0,2,3 emit x0*c0 before x1*c1 (x87 operand order). class (a). */
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
typedef void (*VCb)(int, float *, float *, float *);
// MATCH: 0x0055c2d0 FUN_0055c2d0
void FUN_0055c2d0(int param_1, float *param_2, float *param_3, float *param_4, float *param_5)
{
  float v[4];

  v[0] = 0.0f;
  if (param_2 != (float *)0) {
    v[0] = param_2[0xe] * param_3[2] + (param_2[0xd] * param_3[1] + param_2[0xc] * *param_3);
    v[1] = param_2[2] * param_3[2] + (param_2[1] * param_3[1] + param_2[0] * *param_3);
    v[2] = param_2[6] * param_3[2] + (param_2[5] * param_3[1] + param_2[4] * *param_3);
    v[3] = param_2[10] * param_3[2] + (param_2[9] * param_3[1] + param_2[8] * *param_3);
    param_3 = &v[1];
  }
  (*(VCb *)(*(int *)(param_1 + 0x5c) + 0x20))(param_1, param_3, param_4, param_5);
  *param_4 = v[0] + *param_4;
  *param_5 = v[0] + *param_5;
}
