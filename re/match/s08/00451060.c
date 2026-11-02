// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00451060: 100% by hand (raw 80.0%). FUN_00467210 takes one argument (add esp, 4); the first
   push 0 is FUN_004c1480's third argument. */
/* ---- MATCH-S08 preamble (same text in every re/match/s08 source; not part of any function body) ----
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
int FUN_00467210(int p0);
char *FUN_004840d0(int p0);
int FUN_004c1480(int p0, int p1, int p2);
// MATCH: 0x00451060 FUN_00451060
void FUN_00451060(void)
{
  int frame;

  frame = *(int *)(FUN_004840d0(0) + 4);
  FUN_004c1480(frame, FUN_00467210(0), 0);
}
