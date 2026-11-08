// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x005578a0: hand-matched (h1). 100%. Callee globals declared as functions, RpAtomicRegisterPlugin is really 0x004e7d40 (5 args), 'x >= 0' spelling for the setge. */
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
MGLOBAL(int, DAT_00913270, 0x00913270);
MGLOBAL(int, DAT_00913274, 0x00913274);
MGLOBAL(int, DAT_00913278, 0x00913278);
MGLOBAL(int, DAT_0091327c, 0x0091327c);
int FUN_00557bf0(void);
int FUN_00557c60(void);
int FUN_00557cb0(void);
int FUN_00557d90(void);
int FUN_00557e50(void);
int FUN_00557e90(void);
int FUN_00561f40(void);
int FUN_00557920(void);
int FUN_004c2d90(int p0, int p1, int p2, int p3);
void FUN_004e7da0(int p0, int p1, int p2, int p3);
int FUN_004e7d40(int p0, int p1, int p2, int p3, int p4);
// MATCH: 0x005578a0 FUN_005578a0
int FUN_005578a0(void)
{
  DAT_00913270 = FUN_004c2d90(0,0x112,(int)FUN_00557e50,(int)FUN_00557e90);
  DAT_00913274 = FUN_004e7d40(4,0x112,(int)FUN_00561f40,(int)FUN_00557d90,(int)FUN_00557cb0);
  FUN_004e7da0(0x112,(int)FUN_00557920,(int)FUN_00557bf0,(int)FUN_00557c60);
  DAT_00913278 = 0;
  DAT_0091327c = 0;
  return DAT_00913274 >= 0;
}
