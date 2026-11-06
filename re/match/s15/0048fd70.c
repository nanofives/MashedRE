// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x0048fd70: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs15_20261102/results.tsv. */
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
typedef unsigned char undefined1;
typedef int code();
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(undefined1, DAT_007668f8, 0x007668f8);
MGLOBAL(undefined1, DAT_00766978, 0x00766978);
MGLOBAL(undefined1, DAT_00766f00, 0x00766f00);
MGLOBAL(undefined1, DAT_0076d900, 0x0076d900);
MGLOBAL(undefined1, DAT_0076d940, 0x0076d940);
MGLOBAL(undefined1, DAT_007706d8, 0x007706d8);
void FUN_004770a0(int p0);
// MATCH: 0x0048fd70 FUN_0048fd70

void FUN_0048fd70(void)

{
  (**(code **)(DAT_007d3ff8 + 0x20))(8,0);
  FUN_004770a0(&DAT_0076d900);
  FUN_004770a0(&DAT_0076d940);
  FUN_004770a0(&DAT_00766978);
  FUN_004770a0(&DAT_007668f8);
  FUN_004770a0(&DAT_00766f00);
  FUN_004770a0(&DAT_007706d8);
  (**(code **)(DAT_007d3ff8 + 0x20))(8,1);
  return;
}
