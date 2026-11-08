// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x00552da0 (h2): 100% (regs 100%), byte-identical. Builds {x, y, 1.0f} on the stack and calls
   FUN_004c5010(stack[top], &v, 1) on the matrix stack at 0x00912a84 (index 0x00912b04), then zeroes
   0x00912bd8 and 0x00912bec. The raw converter passed the vector as three int locals (42.1%). */
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
typedef struct { float x, y, z; } V3d;
typedef struct { int m[16]; } Matrix64;
MGLOBAL(int, DAT_00912b04, 0x00912b04);
MGLOBAL(int, DAT_00912bd8, 0x00912bd8);
MGLOBAL(int, DAT_00912bec, 0x00912bec);
MGLOBALA(Matrix64 *, DAT_00912a84, 32, 0x00912a84);
Matrix64 *FUN_004c5010(Matrix64 *m, V3d *v, int op);
// MATCH: 0x00552da0 FUN_00552da0
int FUN_00552da0(float x, float y)
{
  V3d v;

  v.x = x;
  v.y = y;
  v.z = 1.0f;
  FUN_004c5010(DAT_00912a84[DAT_00912b04], &v, 1);
  DAT_00912bd8 = 0;
  DAT_00912bec = 0;
  return 1;
}
