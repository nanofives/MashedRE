// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x00475010: 100% (regs 100%), relocheck OK. By hand (raw 60.0%): a {callback, data} block whose data is
   &param_2, passed to FUN_00474fb0; returns param_1 (kept in esi). */
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
typedef struct { void *cb; void *data; } CBData;
void *FUN_00474fb0(void *p0, void *p1);
void *FUN_00474f50(void *atomic, void *data);
// MATCH: 0x00475010 FUN_00475010
void *FUN_00475010(void *param_1, int param_2)
{
  CBData s;
  s.cb = (void *)FUN_00474f50;
  s.data = &param_2;
  FUN_00474fb0(param_1, &s);
  return param_1;
}
