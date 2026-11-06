// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x00467110 (h1): 100% (regs 100%), relocheck OK. Holds static callee 0x004670a0 (100%): its two
   args arrive in EDI/ESI (pilot finding 1: static callee defined before its caller, /Ob1). First hand pass. */
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
MGLOBAL(int, DAT_006905b0, 0x006905b0);
MGLOBAL(int, DAT_006905b4, 0x006905b4);
MGLOBAL(int, DAT_006905b8, 0x006905b8);
void FUN_00467020(int param_1);
void FUN_004c0740(int param_1, int param_2);
int FUN_004c0b30(void);
int FUN_004c1d30(void);
int FUN_004c77c0(int param_1, int param_2, int param_3, int param_4);
int FUN_004c1a40(int param_1, float param_2);
int FUN_004c1b10(int param_1, float param_2);
int FUN_004c1c80(int param_1, float *param_2);
int FUN_004c2bf0(int param_1);

// MATCH: 0x004670a0 FUN_004670a0
static int FUN_004670a0(int param_1, int param_2)
{
  DAT_006905b0 = FUN_004c1d30();
  if (DAT_006905b0 != 0) {
    FUN_004c0740(DAT_006905b0, FUN_004c0b30());
    *(int *)(DAT_006905b0 + 0x60) = FUN_004c77c0(param_1, param_2, 0, 2);
    *(int *)(DAT_006905b0 + 100) = FUN_004c77c0(param_1, param_2, 0, 1);
    if ((*(int *)(DAT_006905b0 + 4) != 0) && (*(int *)(DAT_006905b0 + 0x60) != 0) &&
        (*(int *)(DAT_006905b0 + 100) != 0)) {
      return DAT_006905b0;
    }
  }
  FUN_00467020(DAT_006905b0);
  return 0;
}

// MATCH: 0x00467110 FUN_00467110
void FUN_00467110(int param_1, int param_2)
{
  float local_8[2];

  FUN_004670a0(param_1, param_2);
  local_8[0] = 0.8f;
  local_8[1] = 0.8f;
  FUN_004c1c80(DAT_006905b0, local_8);
  FUN_004c1a40(DAT_006905b0, 0.1f);
  FUN_004c1b10(DAT_006905b0, 180.0f);
  DAT_006905b4 = FUN_004c2bf0(DAT_006905b0);
  DAT_006905b8 = FUN_004c0b30();
  FUN_004c0740(DAT_006905b4, DAT_006905b8);
}
