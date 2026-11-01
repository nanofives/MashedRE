// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x0042d290 (h5, by hand): 100.0% (regs 100.0%). Two sprintf (FUN_004a2b60, cdecl varargs) of an int and a float->int (_ftol) with a zero pad below 10. Residual: none. */
/* ---- MATCH-S07 preamble (same text in every re/match/s07 source; not part of any function body) ----
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
MGLOBALA(char, s_d_005cd794, 4, 0x005cd794);
MGLOBALA(char, s_sd_005cd798, 8, 0x005cd798);
MGLOBALA(char, s_0_005cd7a0, 4, 0x005cd7a0);
int FUN_004a2b60(char * p0, const char * p1, ...);
// MATCH: 0x0042d290 FUN_0042d290
void FUN_0042d290(int param_1, float param_2, char * param_3, char * param_4)
{
  int iVar1;

  if (param_1 < 10) {
    FUN_004a2b60(param_3, s_sd_005cd798, s_0_005cd7a0, param_1);
  }
  else {
    FUN_004a2b60(param_3, s_d_005cd794, param_1);
  }
  iVar1 = (int)param_2;
  if (iVar1 < 10) {
    FUN_004a2b60(param_4, s_sd_005cd798, s_0_005cd7a0, iVar1);
  }
  else {
    FUN_004a2b60(param_4, s_d_005cd794, iVar1);
  }
}
