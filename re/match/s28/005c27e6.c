// FLAGS: /O1
/* MATCH-S28 h1 0x005c27e6 : 100.0% (regs 100). By hand: raw g2s had FUN_005c31d0 with 2 params; real call is cdecl with 5 args (buf,str,len,0,0), zeros are the isspace result reused (while loop form gives the same). _isspace/_strlen called as FUN_004ac7f0/FUN_004a9410. Residual: none. */

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

typedef unsigned int size_t;
int FUN_005c31d0(unsigned int * p0, char *p1, int p2, int p3, int p4);
int FUN_004ac7f0(int p0);
size_t FUN_004a9410(char * p0);
// MATCH: 0x005c27e6 FUN_005c27e6
double __cdecl FUN_005c27e6(char *_String)
{
  unsigned int local_1c[6];
  int iVar1;

  while (FUN_004ac7f0((unsigned int)(unsigned char)*_String)) {
    _String = _String + 1;
  }
  iVar1 = FUN_005c31d0(local_1c,_String,FUN_004a9410(_String),0,0);
  return *(double *)(iVar1 + 0x10);
}


