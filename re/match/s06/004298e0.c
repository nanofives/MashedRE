// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x004298e0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs06_20261101/results.tsv.
   C++ check fixed (h12, 2026-11-01): FUN_00411580 p1..p3 undefined4* (addresses of locals). Prototypes only, codegen unchanged (100%, relocheck OK). */
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
MGLOBAL(undefined1, DAT_0067d99c, 0x0067d99c);
MGLOBAL(undefined4, DAT_0067d994, 0x0067d994);
MGLOBAL(undefined4, DAT_0067d98c, 0x0067d98c);
MGLOBAL(undefined4, DAT_008991bc, 0x008991bc);
undefined4 FUN_00411580(int p0, undefined4 *p1, undefined4 *p2, undefined4 *p3);
void FUN_00430b00(int p0, int p1, undefined4 p2, undefined4 p3, undefined4 p4);
MGLOBAL(int, _DAT_0067d99c, 0x0067d99c);
// MATCH: 0x004298e0 FUN_004298e0
/* [C1 2026-06-01] Resets the HUD lap-time display to a max/sentinel split for two display slots (0
   and 1). */

void FUN_004298e0(void)

{
  undefined4 local_c;
  undefined4 local_8;
  undefined4 local_4;
  
  _DAT_0067d99c = 0x3f7d70a4;
  DAT_0067d994 = 0x3b;
  DAT_0067d98c = 9;
  DAT_008991bc = 0;
  local_4 = 0x3f7d70a4;
  local_8 = 0x3b;
  local_c = 9;
  FUN_00430b00(0,0,0x3f7d70a4,0x3b,9);
  FUN_00411580(0,&local_4,&local_8,&local_c);
  FUN_00430b00(0,1,local_4,local_8,local_c);
  local_4 = 0x3f7d70a4;
  local_8 = 0x3b;
  local_c = 9;
  FUN_00430b00(1,0,0x3f7d70a4,0x3b,9);
  FUN_00411580(1,&local_4,&local_8,&local_c);
  FUN_00430b00(1,1,local_4,local_8,local_c);
  return;
}
