// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00495150 (h1): 100% (regs 100%), by hand. Residual: none (whole symbol byte-identical, jump
   table included). What moved it (77% -> 100%): an explicit `case 0:` with its own body next to `default:`.
   Without it 9176 rebases the table (dec eax / cmp 4). The original's table has 6 entries from 0, entry 0 =
   default. Callee arities from the caller's add esp. */
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
MGLOBAL(undefined4, DAT_006147c0, 0x006147c0);
MGLOBAL(int, DAT_007719e4, 0x007719e4);
MGLOBAL(int, DAT_007719e8, 0x007719e8);
undefined4 FUN_00495120(void);
void FUN_004955b0(void);
undefined4 FUN_00499710(void);
void FUN_004960a0(undefined4 param_1);
void FUN_004963b0(undefined4 param_1);
void FUN_00498510(undefined4 param_1, undefined4 param_2);
// MATCH: 0x00495150 FUN_00495150
int FUN_00495150(void)
{
  undefined4 mode;
  FUN_00495120();
  FUN_004955b0();
  FUN_004960a0(FUN_00499710());
  FUN_004963b0(FUN_00499710());
  switch (DAT_007719e8) {
  case 1:
    mode = 2;
    break;
  case 2:
    mode = 3;
    break;
  case 3:
    mode = 4;
    break;
  case 4:
    mode = 5;
    break;
  case 5:
    mode = 1;
    break;
  case 0:
    mode = 0;
    break;
  default:
    mode = 0;
    break;
  }
  FUN_00498510(DAT_006147c0, mode);
  return DAT_007719e4 == 0;
}
