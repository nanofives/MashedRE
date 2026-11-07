// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 h3 0x005ab010: by hand. Best 50.0% (regs 61.1%). Residual (b): original compares two 64-bit values with hi halves xor'd to 0 (cmp ecx,edx; ja; jb; cmp eax,esi; jae) and does 32-bit sub / (-1 - g) add. 64-bit casts give the compare but in swapped operand order and without the unfolded zero halves. Tried ulonglong temps, (ulonglong) casts on either side, a>=b / b<=a spellings. */
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
MGLOBAL(uint, DAT_007dcdf0, 0x007dcdf0);
uint FUN_005ab040(void);
// MATCH: 0x005ab010 FUN_005ab010
int FUN_005ab010(void)
{
  ulonglong a = FUN_005ab040();
  if ((ulonglong)DAT_007dcdf0 <= a) {
    return (int)(a - DAT_007dcdf0);
  }
  return (int)(a + (-1 - DAT_007dcdf0));
}
