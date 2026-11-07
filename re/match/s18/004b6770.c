// FLAGS: /O2 /GS /Ob1
/* MATCH-S18 0x004b6770 (h4): 100%, regs 100%. By hand (g2s did not compile: FILE type). fclose is FUN_004a4368 (CRT, called by address), else CloseHandle. */
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
MGLOBAL(unsigned char, DAT_007d3e50, 0x007d3e50);
MGLOBAL(void *, DAT_007d3e48, 0x007d3e48);
int FUN_004a4368(void *p0);
__declspec(dllimport) int __stdcall CloseHandle(void *);

// MATCH: 0x004b6770 FUN_004b6770
void FUN_004b6770(void)
{
  if (DAT_007d3e50 & 1) {
    FUN_004a4368(DAT_007d3e48);
    DAT_007d3e48 = 0;
    return;
  }
  CloseHandle(DAT_007d3e48);
}
