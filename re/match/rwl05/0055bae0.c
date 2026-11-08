// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x0055bae0: hand-matched (h1). 39.2% (regs 39.2%, ops 82.4%), 44/51 ins. Point-transform by 4x3 matrix. Original keeps one sum spilled to [esp+8] and pops two dead st(0); no spelling (locals vs raw, term order, /G6 /Ox /O1 /GS, permuter 45%) reproduced it. Class (a) x87 scheduling / (b). */
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
typedef float * (__cdecl *fnv)(int, int);
// MATCH: 0x0055bae0 FUN_0055bae0
void FUN_0055bae0(int param_1,float *param_2,int param_3)
{
  float *p;
  float a, b, c;

  p = (*(fnv *)(*(int *)(param_1 + 0x5c) + 8))(param_1,param_3);
  if ((p != 0) && (param_2 != 0)) {
    a = param_2[8] * p[2] + param_2[4] * p[1] + param_2[0] * p[0];
    b = param_2[9] * p[2] + param_2[1] * p[0] + param_2[5] * p[1];
    c = param_2[10] * p[2] + param_2[2] * p[0] + param_2[6] * p[1];
    p[0] = param_2[0xc] + a;
    p[1] = param_2[0xd] + b;
    p[2] = param_2[0xe] + c;
  }
}
