// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x00475830: 100% (regs 100%), relocheck OK. By hand (raw 90.1%): both args typed as a struct with an int at +0x18,
   DAT_006915fc typed float (FUN_00558030 takes a float). */
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
typedef struct { unsigned char pad[0x18]; int v18; } Obj18;
MGLOBAL(float, DAT_006915fc, 0x006915fc);
int FUN_00557ec0(Obj18 *p0, int p1, int p2);
int FUN_00558030(Obj18 *p0, float p1);
int FUN_00558100(Obj18 *p0);
// MATCH: 0x00475830 FUN_00475830
Obj18 *FUN_00475830(Obj18 *param_1, Obj18 *param_2)
{
  FUN_00557ec0(param_1, 0, param_1->v18);
  FUN_00557ec0(param_1, 1, param_1->v18);
  FUN_00557ec0(param_1, 2, param_1->v18);
  FUN_00557ec0(param_1, 3, param_1->v18);
  FUN_00557ec0(param_1, 4, param_1->v18);
  FUN_00557ec0(param_1, 5, param_2->v18);
  FUN_00557ec0(param_1, 6, param_2->v18);
  FUN_00557ec0(param_1, 7, param_2->v18);
  FUN_00557ec0(param_1, 8, param_2->v18);
  FUN_00557ec0(param_1, 9, param_2->v18);
  FUN_00558100(param_1);
  FUN_00558030(param_1, DAT_006915fc);
  return param_1;
}
