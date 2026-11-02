// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x0042a6b0 (h2, by hand): 100.0% (regs 100.0%). Raw converter 65.0%: Ghidra gave the two tail
   callees 0x004b3d80 / 0x004b3d20 no arguments (each takes one stack argument, &DAT_0067e1a8), and returned
   from each branch. The original keeps a result variable set to 0 up front (xor edi,edi) and assigns it in
   if (found == 1) ... else if (found != 0) ...; return result; the compiler still turns both assignments into
   tail jumps. Writing direct returns gave 70.0% (no edi). Same family as 0x0042a5d0 (two-way loader).
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
MGLOBAL(char *, PTR_DAT_005f65b8, 0x005f65b8);
MGLOBAL(char, DAT_0067dfa8, 0x0067dfa8);
MGLOBAL(char, DAT_0067e1a8, 0x0067e1a8);
int FUN_0042a530(char *p0, char *p1);
int FUN_004b3d20(char *p0);
int FUN_004b3d80(char *p0);
char *strchr(const char *p0, int p1);
// MATCH: 0x0042a6b0 FUN_0042a6b0
int FUN_0042a6b0(char *param_1)
{
  int result;
  int found;

  result = 0;
  (*(RwStrFn *)(DAT_007d3ff8 + 0xcc))(&DAT_0067dfa8, param_1);
  if (strchr(param_1, '.') == 0) {
    (*(RwStrFn *)(DAT_007d3ff8 + 0xd4))(&DAT_0067dfa8, PTR_DAT_005f65b8);
  }
  found = FUN_0042a530(&DAT_0067e1a8, &DAT_0067dfa8);
  if (found == 1) {
    result = FUN_004b3d80(&DAT_0067e1a8);
  } else if (found != 0) {
    result = FUN_004b3d20(&DAT_0067e1a8);
  }
  return result;
}
