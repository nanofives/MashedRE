// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
// h2 hand-matched. 83.3% (regs 86.1%, ops 92.6%). Tri-strip to tri-list copy, param_4 != 0 (offset) branch first, odd-index branch first. Residual: original tests the skip-all case as test ecx,ecx; jbe on cnt = n-2 and keeps out=0 in edx/[esp+0x1c], ours compares n with 2 and re-materialises 0. cnt != 0 / 0 < cnt forms and a 240 s permuter run did not help. Class (b).
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

// MATCH: 0x004ebb00 FUN_004ebb00
int FUN_004ebb00(short *param_1, short *param_2, int param_3, int param_4)
{
  uint cnt = param_3 - 2;
  uint i;
  int out = 0;

  if (param_4 != 0) {
    for (i = 0; i < cnt; i++) {
      short a = *param_2;
      if (a != param_2[1] && a != param_2[2] && param_2[1] != param_2[2]) {
        short o = (short)param_4;
        if ((i & 1) != 0) {
          param_1[2] = a - o;
          param_1[1] = param_2[1] - o;
          param_1[0] = param_2[2] - o;
        } else {
          param_1[0] = a - o;
          param_1[1] = param_2[1] - o;
          param_1[2] = param_2[2] - o;
        }
        param_1 += 3;
        out += 3;
      }
      param_2++;
    }
  }
  else {
    for (i = 0; i < cnt; i++) {
      short a = *param_2;
      if (a != param_2[1] && a != param_2[2] && param_2[1] != param_2[2]) {
        if ((i & 1) != 0) {
          param_1[2] = a;
          param_1[1] = param_2[1];
          param_1[0] = param_2[2];
        } else {
          param_1[0] = a;
          param_1[1] = param_2[1];
          param_1[2] = param_2[2];
        }
        param_1 += 3;
        out += 3;
      }
      param_2++;
    }
  }
  return out;
}
