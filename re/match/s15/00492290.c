// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 h6 0x00492290: 100% (regs 100%). Hand: FUN_00499690 returns int (raw bool gave test al,al). Residual: none. */
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
MGLOBAL(int, DAT_00828300, 0x00828300);
void FUN_004926c0(void);
int FUN_00492770(void);
int FUN_004929d0(void);
int FUN_00492d20(void);
int FUN_00492d30(void);
int FUN_00492e90(void);
int FUN_00493480(void);
int FUN_00499690(void);
// MATCH: 0x00492290 FUN_00492290
int FUN_00492290(void)
{
  FUN_00492770();
  while (DAT_00828300 == 0 && FUN_00499690() == 0) {
    FUN_004929d0();
    FUN_00492d20();
    FUN_00492d30();
    FUN_00492e90();
    FUN_004926c0();
    FUN_00493480();
  }
  return 1;
}
