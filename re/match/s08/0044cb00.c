// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x0044cb00: 100% by hand (raw 77.5%). The fstp/mov/push of the angle comes from a float local
   assigned before the call (a cast inside the argument list gives fstp [esp]); the FUN_00442410 result in a
   local keeps push 2 after that call and lets the three calls share one add esp, 0x28. Same shape as the
   0x00448dc0 twin (5 entries at 0x00896460). */
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
MGLOBAL(undefined1, DAT_006146fc, 0x006146fc);
MGLOBALA(int, DAT_00896230, 4, 0x00896230);
undefined * FUN_00442410(int p0);
float10 FUN_00442420(int p0);
int FUN_004c1340(int p0, void *p1, int p2);
int FUN_004c1520(int p0, void *p1, float p2, int p3);
void FUN_004c15c0(int p0);
// MATCH: 0x0044cb00 FUN_0044cb00
void FUN_0044cb00(void)
{
  int i;
  int *p;
  int n;
  int frame;
  float a;
  undefined *v;

  i = 1;
  p = DAT_00896230;
  n = 4;
  do {
    frame = *(int *)(*p + 4);
    FUN_004c15c0(frame);
    a = (float)FUN_00442420(i);
    FUN_004c1520(frame, &DAT_006146fc, a, 0);
    v = FUN_00442410(i);
    FUN_004c1340(*(int *)(*p + 4), v, 2);
    i = i + 1;
    p = p + 1;
    n = n - 1;
  } while (n != 0);
}
