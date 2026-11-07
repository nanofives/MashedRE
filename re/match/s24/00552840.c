// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x00552840 (h1): 100% (regs 100%). RwV3d axis {0,0,1} local + FUN_004c4d20 (cdecl, 4 args). Array length of DAT_00912a84 unknown, declared 1. */
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
typedef struct RwV3d { float x, y, z; } RwV3d;
void *FUN_004c4d20(void *matrix, const RwV3d *axis, float angle, int combine);
MGLOBAL(int, DAT_00912b04, 0x00912b04);
MGLOBALA(void *, DAT_00912a84, 1, 0x00912a84);
MGLOBAL(int, DAT_00912bd8, 0x00912bd8);
MGLOBAL(int, DAT_00912bec, 0x00912bec);

// MATCH: 0x00552840 FUN_00552840
int FUN_00552840(float param_1)
{
  RwV3d axis;

  axis.x = 0.0f;
  axis.y = 0.0f;
  axis.z = 1.0f;
  FUN_004c4d20(DAT_00912a84[DAT_00912b04], &axis, param_1, 1);
  DAT_00912bd8 = 0;
  DAT_00912bec = 0;
  return 1;
}
