// FLAGS: /O2 /GS /Ob1
// h11: 85.7% (regs 85.7%, ops 100%). Only residual: fld/fmul operand order (original loads param_1 then fmul [0x5cc728], ours loads the global first).
/* MATCH-S13 0x00482900: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs13_20261102/results.tsv. */
/* ---- MATCH-S13 preamble (same text in every re/match/s13 source; not part of any function body) ----
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
MGLOBAL(float, DAT_005cc728, 0x005cc728);
MGLOBALA(char, s_Replay_size_is__d_005cf24c, 19, 0x005cf24c);
void FUN_004987b0(const char *fmt, ...);
// MATCH: 0x00482900 FUN_00482900
int FUN_00482900(float param_1, int param_2)
{
  int n;

  n = (int)(DAT_005cc728 * param_1 / param_2);
  n = n * 0x24 + 0x19c;
  FUN_004987b0(s_Replay_size_is__d_005cf24c, n);
  return n;
}
