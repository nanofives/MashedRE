// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x00562600: hand-matched (h1). 100%. FUN_00562ac0 declared as a function (not a byte global), flags /G5 instead of /GS. */
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
MGLOBAL(int, DAT_005e52a8, 0x005e52a8);
int FUN_00562ac0(void);
int * FUN_00564130(int p0, unsigned p1, unsigned p2);
void FUN_0055c380(int * p0, int * p1);
// MATCH: 0x00562600 FUN_00562600
int * FUN_00562600(int param_1, int param_2)
{
  int *p = FUN_00564130(0x60,0x10,0x30900);
  FUN_0055c380(p,&DAT_005e52a8);
  p[0x10] = param_1;
  p[0x11] = param_2;
  p[0x12] = (int)FUN_00562ac0;
  return p;
}
