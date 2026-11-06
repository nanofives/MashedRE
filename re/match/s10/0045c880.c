// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x0045c880 (h2): 100% (regs 100%). Raw 12.5%. Moves: (1) every switch case assigns all four
   locals in the order a,b,d,c and cases are written in the original's block order 3,0xf,0x13,10/0x17,0x1a,default
   (gives the per-case fld runs with shared tails); (2) the d-term `i != param_3 && j == param_2` sits in BOTH
   branches of `if (param_3 != -1)`, so the -1 branch folds to the original's `cmp ebx,-6`; (3) 6x6 float matrix
   indexed [row][col], int loop counters. */
/* ---- MATCH-S10 preamble (same text in every re/match/s10 source; not part of any function body) ----
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
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005cc344, 0x005cc344);
MGLOBAL(float, DAT_005cc358, 0x005cc358);
MGLOBAL(float, DAT_005cd0fc, 0x005cd0fc);
MGLOBAL(float, DAT_005ce4b8, 0x005ce4b8);
MGLOBAL(float, DAT_005cd30c, 0x005cd30c);
MGLOBAL(float, DAT_005cc56c, 0x005cc56c);
MGLOBAL(float, DAT_005cd0b0, 0x005cd0b0);
MGLOBAL(float, DAT_005cc8f0, 0x005cc8f0);
MGLOBAL(float, DAT_005ce654, 0x005ce654);
MGLOBAL(float, DAT_005cc9c0, 0x005cc9c0);
typedef float Row6[6];
MGLOBALA(Row6, DAT_0088f5e0, 6, 0x0088f5e0);
// MATCH: 0x0045c880 FUN_0045c880
void FUN_0045c880(int param_1, int param_2, int param_3)
{
  float a;
  float b;
  float c;
  float d;
  int i;
  int j;

  switch (param_1) {
  case 3:
    a = DAT_005ce654; b = DAT_005d757c; d = DAT_005cc9c0; c = DAT_005d757c;
    break;
  case 0xf:
    a = DAT_005cd0b0; b = DAT_005d757c; d = DAT_005cc8f0; c = DAT_005d757c;
    break;
  case 0x13:
    a = DAT_005cd30c; b = DAT_005cd30c; d = DAT_005cc56c; c = DAT_005cc56c;
    break;
  case 10:
  case 0x17:
    a = DAT_005ce4b8; b = DAT_005d757c; d = DAT_005d757c; c = DAT_005d757c;
    break;
  case 0x1a:
    a = DAT_005d757c; b = DAT_005d757c; d = DAT_005cd0fc; c = DAT_005d757c;
    break;
  default:
    a = DAT_005d757c; b = DAT_005d757c; d = DAT_005d757c; c = DAT_005d757c;
    break;
  }
  for (i = 0; i < 6; i++) {
    for (j = 0; j < 6; j++) {
      if (param_3 != -1) {
        if (i == param_3 && j == param_2)
          DAT_0088f5e0[param_3][param_2] += a;
        if (i == param_2) {
          if (j == param_3)
            DAT_0088f5e0[param_2][param_3] += b;
        } else if (j == param_3) {
          DAT_0088f5e0[i][param_3] += c;
        }
        if (i != param_3 && j == param_2)
          DAT_0088f5e0[i][param_2] += d;
      } else {
        if (i != param_3 && j == param_2)
          DAT_0088f5e0[i][param_2] += d;
      }
      if (DAT_0088f5e0[i][j] < DAT_005cc344)
        DAT_0088f5e0[i][j] = -5.0f;
      else if (DAT_0088f5e0[i][j] > DAT_005cc358)
        DAT_0088f5e0[i][j] = 5.0f;
    }
  }
}
