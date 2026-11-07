// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 0x005b2de0: 80.4% (regs 93.5%). Hand: Ghidra's 7-arg FUN_005b2fd0 is really 5 args and FUN_005ab1c0 has 4 (nested call, pushes hoisted: FUN_005ab1c0(sub, FUN_005b2fd0(..), FUN_005b2e70, p)); Obj struct with flags at +0x188. Residual (b): original tests [esi+0x188] straight from memory then reloads it for the or (no CSE of the flag load); int/volatile/|= forms, permuter 105 compiles did not reproduce. */
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
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef struct Obj { char pad0[0x28]; char sub[0xf0]; int f118; char pad2[0x6c]; uint flags; char pad3[4]; int f190; } Obj;
void FUN_005b2e70(void);
void FUN_005ab1c0(void *p0, int p1, void *p2, int p3);
int FUN_005ab370(void *p0);
int FUN_005b2fd0(int p0, uint p1, int p2, int p3, int *p4);
// MATCH: 0x005b2de0 FUN_005b2de0
undefined4 FUN_005b2de0(int *param_1)
{
  Obj *o;

  o = (Obj *)*param_1;
  if ((o->flags & 0x20000) != 0) {
    return 0;
  }
  o->flags = o->flags | 0x20000;
  o->f190 = FUN_005ab370(o->sub);
  param_1[8] = 0;
  o->flags = o->flags | 0x40000;
  FUN_005ab1c0(o->sub, FUN_005b2fd0(o->f118, param_1[2], param_1[5], param_1[4], param_1 + 9),
               (void *)FUN_005b2e70, (int)param_1);
  return 1;
}
