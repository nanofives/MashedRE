// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004da430 (h3): 100% (regs 100%). Raw do/while form 75%; 'if (depth > 0) loop else return 1' puts the
   return-1 block at the end as in the original. --bytes: 1 byte at +0x26 = the recursive call rel32 (self-call, not a reloc in the obj). */
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
// MATCH: 0x004da430 FUN_004da430
int FUN_004da430(int param_1,int param_2)
{
  int count = 0;
  int i;
  if (param_1 != 0) {
    if (param_2 > 0) {
      for (i = 0; i < 16; i++) {
        count += FUN_004da430(((int *)(param_1 + 0x1c))[i], param_2 - 1);
      }
    } else {
      return 1;
    }
  }
  return count;
}
