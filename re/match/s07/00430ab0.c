// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x00430ab0 (h9): 100% (regs 100%). Raw g2s 50%: a grouped switch (case 3: case 4: case 5:) makes 9176 emit
   a byte index table plus a jump table, the original has one 9-entry jump table. One case label per return
   statement (identical bodies merged after lowering) gives the single table. No default: out-of-range values fall
   to the ret with eax = DAT_0067e9fc - 2, as in the original. Residual: none. */
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
MGLOBAL(int, DAT_0067e9fc, 0x0067e9fc);
// MATCH: 0x00430ab0 FUN_00430ab0
int FUN_00430ab0(void)
{
  switch (DAT_0067e9fc) {
  case 2: return 0;
  case 3: return 5;
  case 4: return 5;
  case 5: return 5;
  case 6: return 0;
  case 7: return 0;
  case 8: return 0;
  case 9: return 0;
  case 10: return 0;
  }
}
