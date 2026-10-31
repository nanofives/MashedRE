// FLAGS: /O2 /GS /Ob1
/* 0x0040df60: if DAT_0063ba8c in 5..6 and FUN_0042f6a0() in 3..5, switch (DAT_007f0fd0) cases
   4,7,8,9,10 tail-call FUN_00401f10 (jump table at 0x0040df98, 7 entries). Fall-through cases
   (case 4: case 7: ... one call) and an if chain give a compare chain (73.7%); one call per case,
   merged by the optimizer, gives the table. --bytes: 84 B whole symbol identical. Residual: none. */
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
MGLOBAL(int, DAT_0063ba8c, 0x0063ba8c);
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);

int FUN_0042f6a0(void);
void FUN_00401f10(void);

// MATCH: 0x0040df60 FUN_0040df60
void FUN_0040df60(void)
{
  int iVar1;
  if (DAT_0063ba8c >= 5 && DAT_0063ba8c <= 6) {
    iVar1 = FUN_0042f6a0();
    if (iVar1 >= 3 && iVar1 <= 5) {
      switch (DAT_007f0fd0) {
      case 4: FUN_00401f10(); break;
      case 7: FUN_00401f10(); break;
      case 8: FUN_00401f10(); break;
      case 9: FUN_00401f10(); break;
      case 10: FUN_00401f10(); break;
      }
    }
  }
}
