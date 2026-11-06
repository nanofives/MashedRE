// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00494b65 (h1): wrong function boundary, class (c). 0x00494b65 is not a function entry: the real
   function starts at 0x00494b50 (stdcall, 1 arg, ret 4: NULL check -> 0x80004003, then three 16-byte GUID
   compares at +0x2c / +0x00 / +0x10 against 0x005d0c1c / 0x005d0cac / 0x005d0c7c; 0x80070057 on the first
   mismatch, 0x80004005 default, 0 when the last two match). 0x00494b65 is the label after the NULL check.
   This file holds FUN_00494b50, BYTE-IDENTICAL over its whole 99 B (inline memcmp via #pragma intrinsic gives
   the repe cmpsd sequences, xor ebx / xor edx included). match.py cannot score it as is: 0x00494b50 has no row
   in re/console/pc_funcs.csv ("no Ghidra size"). Checked with that table extended by 0x00494b50 = 99 B and
   --bytes. relocheck OK (checked=3, unchecked=0). */
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
MGLOBALA(int, DAT_005d0c1c, 4, 0x005d0c1c);
MGLOBALA(int, DAT_005d0c7c, 4, 0x005d0c7c);
MGLOBALA(int, DAT_005d0cac, 4, 0x005d0cac);
#ifdef __cplusplus
extern "C"
#endif
int __cdecl memcmp(const void *a, const void *b, unsigned int n);
#pragma intrinsic(memcmp)
// MATCH: 0x00494b50 FUN_00494b50
int __stdcall FUN_00494b50(int *param_1)
{
  int r = 0x80004005;
  if (param_1 == 0) {
    return 0x80004003;
  }
  if (memcmp(param_1 + 0xb, DAT_005d0c1c, 16) != 0) {
    return 0x80070057;
  }
  if (memcmp(param_1, DAT_005d0cac, 16) == 0 && memcmp(param_1 + 4, DAT_005d0c7c, 16) == 0) {
    r = 0;
  }
  return r;
}
