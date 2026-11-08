// FLAGS: /O2 /Ob1 /G5
// MATCH-S27 0x005bc750: 100%. Real signature is (int **obj, int *src): the 12-dword copy into a local S12 is field-wise, source statement order found by hill-climb (11.8 -> 100%: 4,3,10,9,11,6,8,2,5,7,0,1) because the original's copy schedule follows source order. COM calls typed __stdcall, int return (hr >= 0). Class: none.
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
typedef unsigned char undefined;
typedef unsigned char undefined1;
MGLOBAL(undefined1, DAT_005d09ac, 0x005d09ac);
typedef struct S12 { int a[12]; } S12;
typedef int (__stdcall *QIFN)(void *, void *, void *);
typedef int (__stdcall *CALLFN)(void *, void *);
typedef int (__stdcall *RELFN)(void *);
// MATCH: 0x005bc750 FUN_005bc750
int FUN_005bc750(int **param_1, int *param_2)
{
  int *iface;
  int hr;
  S12 s;
  int ok;

  iface = 0;
  hr = ((QIFN)*(int *)**param_1)(*param_1, &DAT_005d09ac, &iface);
  if (hr < 0) {
    return 0;
  }
  s.a[4] = param_2[4];
  s.a[3] = param_2[3];
  s.a[10] = param_2[10];
  s.a[9] = param_2[9];
  s.a[11] = param_2[11];
  s.a[6] = param_2[6];
  s.a[8] = param_2[8];
  s.a[2] = param_2[2];
  s.a[5] = param_2[5];
  s.a[7] = param_2[7];
  s.a[0] = param_2[0];
  s.a[1] = param_2[1];
  hr = ((CALLFN)*(int *)(*iface + 0xc))(iface, &s);
  ok = hr >= 0;
  ((RELFN)*(int *)(*iface + 8))(iface);
  return ok;
}
