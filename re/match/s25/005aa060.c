// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 h3 0x005aa060: by hand. Best 85.2% (regs 100.0%). Residual (a): after the call the original keeps param_3 in ecx and the value in edx, ours uses eax/ecx (register naming only, same instruction order). Struct assignment for the 16-byte copy fixed the lea scheduling; 6 spellings of the tail store tried. */
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
typedef struct { undefined4 a; undefined4 b; undefined4 c; undefined4 d; } S25_005aa060;
undefined4 FUN_005aa1e0(undefined4 p0);
void FUN_005aa0c0(undefined4 p0, undefined4 * p1, undefined4 (*p2)(undefined4), S25_005aa060 * p3, undefined4 p4);
// MATCH: 0x005aa060 FUN_005aa060
void FUN_005aa060(undefined4 param_1,undefined4 *param_2,undefined4 *param_3)
{
  S25_005aa060 s;
  s = *(S25_005aa060 *)param_2;
  FUN_005aa0c0(param_1,(undefined4 *)&param_2,FUN_005aa1e0,&s,1);
  if (param_3 != 0) {
    *param_3 = (undefined4)param_2;
  }
}
