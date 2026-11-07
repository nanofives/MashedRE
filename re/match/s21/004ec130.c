// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004ec130 (h1, by hand): 100.0% exact, regs 100.0%. Raw form dropped both callee results and the
   tail call arguments; the original passes (0x88, result == 0) to FUN_004d5480 (2 args, tail jmp). Callee
   0x004e6100 (raw name RpAtomicGetWorldBoundingSphere) takes the int argument, not a float. relocheck OK, C++ ok. */
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
MGLOBAL(int *, DAT_007d3ff8, 0x007d3ff8);
int FUN_004cbb70(int p0, int p1);
int FUN_004cbbd0(int p0, int p1);
void FUN_004d5480(int param_1, int param_2);
int FUN_004e6100(int p0);
// MATCH: 0x004ec130 FUN_004ec130
void FUN_004ec130(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  
  iVar1 = *DAT_007d3ff8;
  if (param_2 == 1) {
    iVar2 = FUN_004cbb70(iVar1,FUN_004e6100(param_1));
  }
  else {
    iVar2 = FUN_004cbbd0(iVar1,param_1 + 0x60);
  }
  FUN_004d5480(0x88,iVar2 == 0);
  return;
}
