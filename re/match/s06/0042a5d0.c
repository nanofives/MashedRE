// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x0042a5d0 (h2, by hand): 100.0% (regs 100.0%). Raw converter 93.1%: Ghidra gave the tail callee
   0x004b3bf0 no arguments. It takes one stack argument (it reads [esp+4] and ends in a plain ret), so the tail call
   is return FUN_004b3bf0(&DAT_0067d9a8); the compiler rewrites [esp+4] and jumps. One of the four 103 B
   siblings 0x0042a5d0/0x0042a640/0x0042a7f0/0x0042a860, same source with different globals and callee.
   DAT_007d3ff8 is RwGlobals*; slots +0xcc / +0xd4 are called with (char *dst, const char *src).
   Residual: none. */
/* ---- MATCH-S06 preamble (same text in every re/match/s06 source; not part of any function body) ----
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
typedef void (*RwStrFn)(char *dst, const char *src);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(char *, PTR_DAT_005f65b0, 0x005f65b0);
MGLOBAL(char, DAT_0067d9a8, 0x0067d9a8);
MGLOBAL(char, DAT_0067e6a8, 0x0067e6a8);
int FUN_0042a530(char *p0, char *p1);
int FUN_004b3bf0(char *p0);
char *strchr(const char *p0, int p1);
// MATCH: 0x0042a5d0 FUN_0042a5d0
int FUN_0042a5d0(char *param_1)
{
  (*(RwStrFn *)(DAT_007d3ff8 + 0xcc))(&DAT_0067e6a8, param_1);
  if (strchr(param_1, '.') == 0) {
    (*(RwStrFn *)(DAT_007d3ff8 + 0xd4))(&DAT_0067e6a8, PTR_DAT_005f65b0);
  }
  if (FUN_0042a530(&DAT_0067d9a8, &DAT_0067e6a8) != 0) {
    return FUN_004b3bf0(&DAT_0067d9a8);
  }
  return 0;
}
