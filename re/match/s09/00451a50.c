// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00451a50 (h1): 100% (regs 100%). Raw converter 75.0%: 64-byte matrix struct copies in and out, callee 0x004c4670 (thunk, called directly) at its own name. Residual: none. */
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
typedef struct { float m[16]; } M16_00451a50;
float * FUN_004c4dc0(M16_00451a50 * p0, M16_00451a50 * p1);
uint * FUN_004c52f0(M16_00451a50 * p0, M16_00451a50 * p1, int p2);
void FUN_004c4670(M16_00451a50 * p0, M16_00451a50 * p1);
// MATCH: 0x00451a50 FUN_00451a50
void FUN_00451a50(int param_1,M16_00451a50 *param_2,M16_00451a50 *param_3)
{
  M16_00451a50 m;
  M16_00451a50 inv;

  m = *param_2;
  FUN_004c4dc0(&inv,(M16_00451a50 *)(param_1 + 0x54));
  FUN_004c52f0(&m,&inv,2);
  FUN_004c52f0(&m,*(M16_00451a50 **)(param_1 + 0x94),2);
  FUN_004c4670(&m,&m);
  *param_3 = m;
}
