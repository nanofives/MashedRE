// FLAGS: /O2 /GS /Ob1
/* 0x0040acd0: switch (DAT_008a9584) 1..6 tail-calls one state handler each (jump table at 0x0040ad04,
   after the ret). The table maps 4 -> FUN_00409e90 and 5 -> FUN_0040a3a0 while the case bodies are laid out
   5 before 4, so the source lists case 5 above case 4. The callees take no argument here (they reuse
   their own [esp+4] slot for their own tail calls). --bytes: 76 B whole symbol incl. table identical.
   Residual: none. */
/* ---- MATCH-S02 preamble (same text in every re/match/s02 source; not part of any function body) ----
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
MGLOBAL(int, DAT_008a9584, 0x008a9584);

void FUN_00409b00(void);
void FUN_0040a130(void);
void FUN_0040a650(void);
void FUN_0040a3a0(void);
void FUN_00409e90(void);
void FUN_00409ad0(void);

// MATCH: 0x0040acd0 FUN_0040acd0
void FUN_0040acd0(void)
{
  switch (DAT_008a9584) {
  case 1:
    FUN_00409b00();
    break;
  case 2:
    FUN_0040a130();
    break;
  case 3:
    FUN_0040a650();
    break;
  case 5:
    FUN_0040a3a0();
    break;
  case 4:
    FUN_00409e90();
    break;
  case 6:
    FUN_00409ad0();
    break;
  }
}
