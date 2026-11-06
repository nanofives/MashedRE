// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x00493ac0 (h8): 100% (regs 100%), 127 B byte-identical. No Ghidra function at this VA (was
   LAB_00493ac0, address taken by FUN_00493b50), so match.py refuses; scored with match.py given size 127
   (scratch/h8/msize.py). /GS frame from char buf[7]. Block order needed the early `return GetACP()` on a failed
   GetLocaleInfoA and `if (cp == 0) return GetACP(); return cp;` (the nested if/return-cp form is 87.8%).
   Residual: none. */
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
__declspec(dllimport) unsigned long __stdcall GetThreadLocale(void);
__declspec(dllimport) int __stdcall GetLocaleInfoA(unsigned long Locale, unsigned long LCType, char *lpLCData, int cchData);
__declspec(dllimport) unsigned int __stdcall GetACP(void);
// MATCH: 0x00493ac0 FUN_00493ac0
unsigned int FUN_00493ac0(void)
{
  char buf[7];
  char *p;
  unsigned long Locale;
  unsigned int cp;

  cp = 0;
  Locale = GetThreadLocale();
  if (GetLocaleInfoA(Locale, 0x1004, buf, 7) == 0) {
    return GetACP();
  }
  for (p = buf; *p != 0; p++) { cp = cp * 10 + *p - 0x30; }
  if (cp == 0) { return GetACP(); } return cp;
}
