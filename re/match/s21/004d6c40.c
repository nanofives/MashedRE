// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d6c40: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
MGLOBAL(int, DAT_007d6bf0, 0x007d6bf0);
MGLOBAL(undefined1, DAT_007d5890, 0x007d5890);
MGLOBAL(int, DAT_007d5894, 0x007d5894);
MGLOBAL(int, DAT_007d6c14, 0x007d6c14);
MGLOBAL(undefined1, DAT_005d8c64, 0x005d8c64);
MGLOBAL(undefined4, DAT_007d5168, 0x007d5168);
MGLOBAL(int, _DAT_007d5890, 0x007d5890);
// MATCH: 0x004d6c40 FUN_004d6c40
/* [C1 2026-05-18] Decompiled signature: `undefined4 FUN_004d6c40(int param_1);` */

undefined4 FUN_004d6c40(int param_1)

{
  if (param_1 != DAT_007d6bf0) {
    _DAT_007d5890 = *(undefined4 *)(&DAT_005d8c64 + param_1 * 4);
    if (DAT_007d5894 == 0) {
      DAT_007d5894 = 1;
      (&DAT_007d5168)[DAT_007d6c14] = 0x13;
      DAT_007d6c14 = DAT_007d6c14 + 1;
    }
    DAT_007d6bf0 = param_1;
  }
  return 1;
}
