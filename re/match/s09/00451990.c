// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00451990 (h1): 100% (regs 100%). Raw converter 44.4%: the range test sets an int flag, then a separate if (flag) picks which 64-byte matrix struct is copied out; a single if with a pointer swap or two returns stayed 84-86%. Residual: none. */
/* ---- MATCH-S09 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
typedef unsigned int uint;
typedef struct { float m[16]; } M16_00451990;
float * FUN_004c4dc0(M16_00451990 * p0, M16_00451990 * p1);
uint * FUN_004c52f0(M16_00451990 * p0, M16_00451990 * p1, int p2);
// MATCH: 0x00451990 FUN_00451990
undefined4 FUN_00451990(int param_1,M16_00451990 *param_2,M16_00451990 *param_3)
{
  M16_00451990 m;
  M16_00451990 inv;
  int inside;

  m = *param_2;
  FUN_004c4dc0(&inv,(M16_00451990 *)(param_1 + 0x54));
  FUN_004c52f0(&m,&inv,2);
  inside = 0;
  if (m.m[12] < *(float *)(param_1 + 0x14) && m.m[12] > *(float *)(param_1 + 0x20) &&
      m.m[14] < *(float *)(param_1 + 0x1c) && m.m[14] > *(float *)(param_1 + 0x28)) {
    inside = 1;
  }
  if (inside) {
    FUN_004c52f0(&m,*(M16_00451990 **)(param_1 + 0x94),2);
    *param_3 = m;
  } else {
    *param_3 = *param_2;
  }
  return inside;
}
