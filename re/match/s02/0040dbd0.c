// FLAGS: /O2 /GS /Ob1
/* 0x0040dbd0 FUN_0040dbd0 (123 B Ghidra; 149 B obj symbol incl. the jump table at 0x0040dc4c and the byte index
   table at 0x0040dc5c): best 100.0% exact, regs 100.0%, whole symbol BYTE-IDENTICAL (match.py --bytes).
   Path: Ghidra form 50% (compare chain) -> one switch with break + DAT_0063ba8c = 5 after it, which the compiler
   tail-duplicates into every case: 100% instructions, but the tables differed. The original's 4-entry table
   is [2, 3..5, 6/10, default(7..9)] where the 3..5 and default entries point at the same code, so cases 3, 4, 5
   are an explicit label whose body equals default's (cross-jumped after the table is built). 7, 8, 9 as an
   explicit group instead gives the right code with the entries in the wrong order. No residual. */
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

void FUN_0041b520(void);
void FUN_0041d910(void);
void FUN_0041e080(void);
int FUN_0042f500(void);
int FUN_0042f6a0(void);
void FUN_0045b350(void);

// MATCH: 0x0040dbd0 FUN_0040dbd0
void FUN_0040dbd0(void)
{
  FUN_0041d910();
  switch (FUN_0042f6a0()) {
  case 2:
    FUN_0041e080();
    break;
  case 3:
  case 4:
  case 5:
    if (FUN_0042f500() != 0) {
      FUN_0045b350();
    } else {
      FUN_0041b520();
    }
    break;
  case 6:
  case 10:
    if (FUN_0042f500() != 0 && DAT_007f0fd0 != 2) {
      FUN_0045b350();
    } else {
      FUN_0041b520();
    }
    break;
  default:
    if (FUN_0042f500() != 0) {
      FUN_0045b350();
    } else {
      FUN_0041b520();
    }
    break;
  }
  DAT_0063ba8c = 5;
}
