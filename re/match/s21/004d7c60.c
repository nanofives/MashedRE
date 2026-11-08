// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d7c60: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs21_20261107/results.tsv. */
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
MGLOBAL(undefined4, DAT_00618424, 0x00618424);
MGLOBAL(undefined4, DAT_00618420, 0x00618420);
MGLOBAL(int, DAT_007d6c50, 0x007d6c50);
MGLOBAL(undefined4, DAT_007d6c58, 0x007d6c58);
MGLOBAL(undefined1, DAT_007d6c2c, 0x007d6c2c);
uint * thunk_FUN_004cc820(int p0, uint p1, uint p2, int p3, uint * p4, uint p5);
// MATCH: 0x004d7c60 FUN_004d7c60

undefined4 FUN_004d7c60(void)

{
  DAT_007d6c50 = thunk_FUN_004cc820(0x3c,DAT_00618420,4,DAT_00618424,&DAT_007d6c2c,0x40000);
  if (DAT_007d6c50 == 0) {
    return 0;
  }
  DAT_007d6c58 = 0;
  return 1;
}
