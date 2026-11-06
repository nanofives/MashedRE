// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 h6 0x00492370: 100% (regs 100%). Hand: three separate early returns (the || form and Ghidra's nesting put the -1 exit at the end); thunk callees called by their own addresses 0x00493540/50/60; FUN_00493540 takes 2 args (one add esp,0x18). Residual: none. */
/* ---- MATCH-S15 preamble (same text in every re/match/s15 source; not part of any function body) ----
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
MGLOBAL(int, DAT_0077195c, 0x0077195c);
MGLOBALA(char, s_Calling_AppInitialiseOnBootup_005cf714, 32, 0x005cf714);
MGLOBALA(char, s_Calling_AppDestroy_005cf700, 20, 0x005cf700);
MGLOBALA(char, s_Calling_SoftwareTidyUpBeforeExiting_005cf6d8, 37, 0x005cf6d8);
MGLOBALA(char, s_Calling_HardwareExitApplication_005cf6b4, 33, 0x005cf6b4);
int FUN_00402750(void);
void FUN_00402a40(void);
int FUN_00492270(void);
int FUN_00492290(void);
int FUN_004924f0(void);
int FUN_00493900(void);
void FUN_004963e0(const char *p0);
void FUN_004996f0(int p0);
int FUN_00499ba0(int p0, int p1, int p2, int p3);
int FUN_00499cc0(void);
int FUN_004c5930(int p0);
int FUN_005c9d00(void);
void FUN_00493550(void);
int FUN_00493540(int p0, int p1);
int FUN_00493560(void);
// MATCH: 0x00492370 FUN_00492370
int __stdcall FUN_00492370(int param_1, int param_2, int param_3, int param_4)
{
  FUN_00499ba0(param_1, param_2, param_3, param_4);
  FUN_00493900();
  if (!FUN_00493540(0, 0)) {
    return -1;
  }
  if (!FUN_004924f0()) {
    return -1;
  }
  if (!FUN_00492270()) {
    return -1;
  }
  FUN_005c9d00();
  FUN_004996f0(param_4);
  FUN_004963e0(s_Calling_AppInitialiseOnBootup_005cf714);
  if (FUN_00402750() != 0) {
    FUN_00492290();
  }
  FUN_004963e0(s_Calling_AppDestroy_005cf700);
  FUN_00402a40();
  if (DAT_0077195c != 0) {
    FUN_004c5930(DAT_0077195c);
    DAT_0077195c = 0;
  }
  FUN_004963e0(s_Calling_SoftwareTidyUpBeforeExiting_005cf6d8);
  FUN_00493550();
  FUN_004963e0(s_Calling_HardwareExitApplication_005cf6b4);
  FUN_00493560();
  return FUN_00499cc0();
}
