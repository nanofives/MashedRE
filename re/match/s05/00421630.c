// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00421630 (95 B): 100.0% (regs 100, ops 100), by hand. Gets matrix 0x0041f220(i), inverts it
   (0x004c4dc0), transforms one point (0x004c3d90) and passes it to 0x004215e0. Locals in declaration order
   point, matrix, inverse give the original frame.
   Residual: none. Relocheck OK. */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
typedef struct V3_00421630 { float x, y, z; } V3_00421630;
typedef struct Mat_00421630 { float m[16]; } Mat_00421630;
void FUN_0041f220(int i, Mat_00421630 *m);
void FUN_004c4dc0(Mat_00421630 *dst, Mat_00421630 *src);
void FUN_004c3d90(V3_00421630 *out, V3_00421630 *in, int n, Mat_00421630 *m);
void FUN_004215e0(int i, float d, V3_00421630 *pos);
// MATCH: 0x00421630 FUN_00421630
void FUN_00421630(int i, float d, V3_00421630 *pt)
{
  V3_00421630 local;
  Mat_00421630 m;
  Mat_00421630 inv;
  FUN_0041f220(i, &m);
  FUN_004c4dc0(&inv, &m);
  FUN_004c3d90(&local, pt, 1, &inv);
  FUN_004215e0(i, d, &local);
}
