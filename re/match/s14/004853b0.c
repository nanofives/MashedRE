// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 0x004853b0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs14_20261102/results.tsv. */
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
typedef unsigned int undefined4;
typedef unsigned int uint;
MGLOBAL(int, DAT_006e71cc, 0x006e71cc);
undefined4 FUN_00485340(uint p0);
#ifdef __cplusplus
void FUN_004987b0(const char *p0, ...); /* port: variadic debug printf (C++ needs the real arity) */
#else
void FUN_004987b0(undefined4 p0);
#endif
// MATCH: 0x004853b0 FUN_004853b0
undefined4 FUN_004853b0(int param_1)

{
  int iVar1;
  
  iVar1 = FUN_00485340(param_1);
  if (iVar1 == 0) {
    FUN_004987b0("SmplFzx : Not an active SmplFzx object %d\n",param_1);
    return 0;
  }
  return *(undefined4 *)(param_1 * 0x10 + *(int *)(DAT_006e71cc + 0xc));
}
