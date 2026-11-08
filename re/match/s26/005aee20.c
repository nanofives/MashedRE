// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005aee20 (h3): 78.6% exact, regs 78.6%. Residual class (b) original keeps the counter in al, copies to cl without and 0x1f, esi=1 hoisted; 5 spellings tried (1<<b, &0x1f, %32, loop forms). */
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
typedef unsigned int uint;
typedef unsigned char byte;
// MATCH: 0x005aee20 FUN_005aee20
void FUN_005aee20(uint param_1)

{
  byte bVar1;
  
  bVar1 = 0;
  do {
    if ((param_1 & 1 << (bVar1 & 0x1f)) != 0) {
      return;
    }
    bVar1 = bVar1 + 1;
  } while (bVar1 < 0x20);
  return;
}
