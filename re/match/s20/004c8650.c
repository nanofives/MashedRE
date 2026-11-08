// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004c8650: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs20_20261107/results.tsv. */
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
MGLOBAL(undefined4, DAT_006181c8, 0x006181c8);
MGLOBAL(undefined4, DAT_006181cc, 0x006181cc);
MGLOBAL(undefined4, DAT_006181d0, 0x006181d0);
MGLOBAL(undefined4, DAT_006181d4, 0x006181d4);
MGLOBAL(undefined4, DAT_006181d8, 0x006181d8);
MGLOBAL(undefined4, DAT_007d40c0, 0x007d40c0);
MGLOBAL(undefined4, DAT_007d40c4, 0x007d40c4);
MGLOBAL(undefined4, DAT_007d40c8, 0x007d40c8);
// MATCH: 0x004c8650 FUN_004c8650
void FUN_004c8650(void)

{
  uint uVar1;
  
  DAT_006181c8 = 0xffffffff;
  DAT_006181cc = 0xffffffff;
  DAT_006181d0 = 0xffffffff;
  DAT_006181d4 = 0xffffffff;
  DAT_006181d8 = 0xffffffff;
  uVar1 = 0;
  do {
    *(undefined4 *)((int)&DAT_007d40c0 + uVar1) = 0xffffffff;
    *(undefined4 *)((int)&DAT_007d40c4 + uVar1) = 0;
    *(undefined4 *)((int)&DAT_007d40c8 + uVar1) = 0;
    uVar1 = uVar1 + 0x10;
  } while (uVar1 < 0x40);
  return;
}
