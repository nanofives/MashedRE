// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x004952f0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs16_20261102/results.tsv. */
/* ---- MATCH-S16 preamble (same text in every re/match/s16 source; not part of any function body) ----
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
typedef unsigned int undefined4;
typedef unsigned int uint;
MGLOBAL(uint, DAT_00616038, 0x00616038);
void FUN_00496400(undefined4 p0);
void FUN_004b65c0(undefined4 * p0);
void __chkstk(void);
void thunk_FUN_004b67a0(void);
// MATCH: 0x004952f0 FUN_004952f0
/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* [C1 2026-06-01] `void FUN_004952f0(void)` — logs and closes the currently-open piz file. */

void FUN_004952f0(void)

{
  uint unaff_retaddr;
  undefined1 local_20b0 [8364];
  uint local_4;
  
  local_4 = DAT_00616038 ^ unaff_retaddr;
  FUN_004b65c0(local_20b0);
  FUN_00496400("Closing piz file %s\n",local_20b0);
  thunk_FUN_004b67a0();
  return;
}
