// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 h1 0x005ae0c0 (166 B). Best 100.0% (regs 100.0%), exact. GUID-like copy; cursor int* local passed by address to FUN_005aeca0(int**,void*,int); G4 struct assigns. */
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
typedef struct G4 { int d[4]; } G4;
void FUN_005aeca0(int **p0, void *p1, int p2);
// MATCH: 0x005ae0c0 FUN_005ae0c0
G4 * FUN_005ae0c0(G4 *param_1,G4 *param_2,int param_3)
{
  G4 L;
  int *cur;
  if (param_3 != 0) {
    cur = L.d;
    FUN_005aeca0(&cur, param_1, 4);
    FUN_005aeca0(&cur, (char *)param_1 + 4, 2);
    FUN_005aeca0(&cur, (char *)param_1 + 6, 2);
    cur[0] = param_1->d[2];
    cur[1] = param_1->d[3];
    *param_2 = L;
    return param_1;
  }
  *param_2 = *param_1;
  return param_1;
}
