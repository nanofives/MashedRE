// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00448ef0: 100% (regs 100%) by hand (h9). Raw 40.5%. The three tick callees get one pushed argument
   (declared int, unused in their bodies). Float global += 0.2f literal (0x005cc9c0), then (sin + 1.0) * 0.15f:
   0x005ce200 is the double 0.15000000596 = (double)0.15f. DAT_008962e0's length is not known (declared [1]). */
/* ---- MATCH-S08 preamble (same text in every re/match/s08 source; not part of any function body) ----
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
MGLOBAL(float, DAT_00683250, 0x00683250);
MGLOBAL(int, DAT_00683248, 0x00683248);
MGLOBALA(int, DAT_008962e0, 1, 0x008962e0);
double sin(double);
#pragma intrinsic(sin)
/* the three tick callees ignore their argument (no read of [esp+4]); this caller pushes one, so their files
   declare it too (int param_1, unused) */
void FUN_00448cb0(int p0);
void FUN_00448dc0(int p0);
void FUN_00448e20(int p0);
int FUN_004671a0(int p0);
void FUN_00488390(int p0, float p1);
void FUN_00491f00(int p0);
// MATCH: 0x00448ef0 FUN_00448ef0
void FUN_00448ef0(int param_1)
{
  FUN_00448cb0(param_1);
  FUN_00448e20(param_1);
  FUN_00448dc0(param_1);
  FUN_00491f00(FUN_004671a0(0));
  DAT_00683250 = DAT_00683250 + 0.2f;
  FUN_00488390(DAT_008962e0[DAT_00683248], (float)((sin(DAT_00683250) + 1.0) * 0.15f));
}
