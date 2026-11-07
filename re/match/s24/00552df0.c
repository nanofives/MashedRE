// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 h3 0x00552df0: 100% (regs 100%). float x,y params into a float v[3] local with v[2]=0. DAT_00912a84 sized 32 (it ends where DAT_00912b04, the index, starts). */
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
MGLOBAL(int, DAT_00912b04, 0x00912b04);
MGLOBAL(int, DAT_00912bd8, 0x00912bd8);
MGLOBAL(int, DAT_00912bec, 0x00912bec);
MGLOBALA(int, DAT_00912a84, 32, 0x00912a84);
void __cdecl FUN_004c51a0(int, float *, int);
// MATCH: 0x00552df0 FUN_00552df0
int FUN_00552df0(float x, float y)
{
  float v[3];
  v[0] = x;
  v[1] = y;
  v[2] = 0.0f;
  FUN_004c51a0(DAT_00912a84[DAT_00912b04], v, 1);
  DAT_00912bd8 = 0;
  DAT_00912bec = 0;
  return 1;
}
