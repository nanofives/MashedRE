// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x0042a470 (h2, by hand): 100.0% (regs 100.0%). Raw converter 77.8%: it folded the four
   prefix cases into one pcVar2 + shared call. Writing each case as its own full cc/d4 call pair lets the
   compiler cross-jump the identical tails, leaving the per-case `push <string>` the original has. The
   "found" test is written as if (found) {...; return 1;} return 0; to put the xor/ret at the end.
   The jump table at 0x0042a518 sits past Ghidra's 167 B size. DAT_007d3ff8 is RwGlobals*; slots +0xcc and
   +0xd4 are called with (char *dst, const char *src). Callee 0x004b5350 is Ghidra's thunk_FUN_00550b00.
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
MGLOBAL(char, DAT_0067daa8, 0x0067daa8);
int FUN_004b5350(char *p0);
// MATCH: 0x0042a470 FUN_0042a470
int FUN_0042a470(char *param_1, const char *param_2, int param_3)
{
  switch (param_3) {
  case 1:
    (*(RwStrFn *)(DAT_007d3ff8 + 0xcc))(&DAT_0067daa8, param_2);
    break;
  case 2:
    (*(RwStrFn *)(DAT_007d3ff8 + 0xcc))(&DAT_0067daa8, "ps2/");
    (*(RwStrFn *)(DAT_007d3ff8 + 0xd4))(&DAT_0067daa8, param_2);
    break;
  case 3:
    (*(RwStrFn *)(DAT_007d3ff8 + 0xcc))(&DAT_0067daa8, "xbox/");
    (*(RwStrFn *)(DAT_007d3ff8 + 0xd4))(&DAT_0067daa8, param_2);
    break;
  case 4:
    (*(RwStrFn *)(DAT_007d3ff8 + 0xcc))(&DAT_0067daa8, "pc/");
    (*(RwStrFn *)(DAT_007d3ff8 + 0xd4))(&DAT_0067daa8, param_2);
    break;
  case 5:
    (*(RwStrFn *)(DAT_007d3ff8 + 0xcc))(&DAT_0067daa8, "gamecube/");
    (*(RwStrFn *)(DAT_007d3ff8 + 0xd4))(&DAT_0067daa8, param_2);
    break;
  }
  if (FUN_004b5350(&DAT_0067daa8) != 0) {
    if (param_1 != 0) {
      (*(RwStrFn *)(DAT_007d3ff8 + 0xcc))(param_1, &DAT_0067daa8);
    }
    return 1;
  }
  return 0;
}
