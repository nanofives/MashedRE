// FLAGS: /O2 /Ob1 /G5
/* h3 hand-match: best 100% (regs 100%), class -. COM Release call is __stdcall (callee pops), not cdecl code* */
/* MATCH-RWL01 0x004e08b0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl01_20261107/results.tsv. */
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
typedef int code();
MGLOBAL(int *, DAT_007d710c, 0x007d710c);
MGLOBAL(int, DAT_007d7118, 0x007d7118);
MGLOBAL(int, DAT_007d7114, 0x007d7114);
MGLOBAL(int, DAT_007d7110, 0x007d7110);
void FUN_004cba80(int p0);
// MATCH: 0x004e08b0 FUN_004e08b0
void FUN_004e08b0(void)

{
  if (DAT_007d710c != (int *)0x0) {
    (*(int (__stdcall *)(int *))*(int *)(*DAT_007d710c + 8))(DAT_007d710c);
    DAT_007d710c = (int *)0x0;
  }
  if (DAT_007d7118 != 0) {
    FUN_004cba80(DAT_007d7118);
    DAT_007d7118 = 0;
  }
  if (DAT_007d7114 != 0) {
    FUN_004cba80(DAT_007d7114);
    DAT_007d7114 = 0;
  }
  if (DAT_007d7110 != 0) {
    FUN_004cba80(DAT_007d7110);
    DAT_007d7110 = 0;
  }
  return;
}
