// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x004f46a0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl03_20261108/results.tsv. */
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
typedef unsigned char undefined;
typedef unsigned char undefined1;
typedef unsigned int undefined4;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_007d7384, 0x007d7384);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d7388, 0x007d7388);
MGLOBAL(undefined4, DAT_007d7378, 0x007d7378);
MGLOBAL(undefined4, DAT_007d737c, 0x007d737c);
MGLOBAL(undefined4, DAT_007d7380, 0x007d7380);
MGLOBAL(int, DAT_007d7394, 0x007d7394);
MGLOBAL(undefined1, DAT_007d738c, 0x007d738c);
MGLOBAL(undefined1, DAT_007d7390, 0x007d7390);
MGLOBAL(undefined1, DAT_007d7398, 0x007d7398);
MGLOBAL(undefined1, DAT_007d739c, 0x007d739c);
MGLOBAL(undefined1, DAT_007d7374, 0x007d7374);
MGLOBAL(float, DAT_006181a0, 0x006181a0);
MGLOBAL(float, DAT_006181a4, 0x006181a4);
MGLOBAL(float, DAT_006181a8, 0x006181a8);
MGLOBAL(undefined1, DAT_006181ac, 0x006181ac);
MGLOBAL(undefined1, DAT_007d7308, 0x007d7308);
MGLOBAL(undefined1, DAT_007d730c, 0x007d730c);
MGLOBAL(undefined1, DAT_007d7310, 0x007d7310);
MGLOBAL(undefined1, DAT_007d7314, 0x007d7314);
MGLOBAL(undefined1, DAT_007d7318, 0x007d7318);
MGLOBAL(undefined1, DAT_007d7348, 0x007d7348);
MGLOBAL(undefined1, DAT_007d734c, 0x007d734c);
MGLOBAL(undefined1, DAT_007d7350, 0x007d7350);
MGLOBAL(undefined1, DAT_007d731c, 0x007d731c);
MGLOBAL(undefined1, DAT_007d7320, 0x007d7320);
MGLOBAL(undefined1, DAT_007d7324, 0x007d7324);
MGLOBAL(undefined1, DAT_007d7328, 0x007d7328);
MGLOBAL(undefined1, DAT_007d732c, 0x007d732c);
MGLOBAL(undefined1, DAT_007d7330, 0x007d7330);
MGLOBAL(undefined1, DAT_007d7334, 0x007d7334);
MGLOBAL(undefined1, DAT_007d7338, 0x007d7338);
MGLOBAL(undefined1, DAT_007d733c, 0x007d733c);
MGLOBAL(undefined1, DAT_007d7340, 0x007d7340);
MGLOBAL(undefined1, DAT_007d7344, 0x007d7344);
MGLOBAL(undefined1, DAT_007d7354, 0x007d7354);
MGLOBAL(undefined1, DAT_007d7358, 0x007d7358);
MGLOBAL(undefined1, DAT_007d735c, 0x007d735c);
MGLOBAL(undefined1, DAT_007d7360, 0x007d7360);
MGLOBAL(undefined1, DAT_007d7364, 0x007d7364);
MGLOBAL(undefined1, DAT_007d7368, 0x007d7368);
MGLOBAL(undefined1, DAT_007d736c, 0x007d736c);
MGLOBAL(undefined1, DAT_007d72a4, 0x007d72a4);
MGLOBAL(undefined1, DAT_007d72a8, 0x007d72a8);
MGLOBAL(undefined1, DAT_007d72ac, 0x007d72ac);
MGLOBAL(undefined1, DAT_007d72b0, 0x007d72b0);
MGLOBAL(undefined1, DAT_007d72b4, 0x007d72b4);
MGLOBAL(undefined1, DAT_007d72b8, 0x007d72b8);
MGLOBAL(undefined1, DAT_007d72bc, 0x007d72bc);
MGLOBAL(undefined1, DAT_007d72c0, 0x007d72c0);
MGLOBAL(undefined1, DAT_007d72c4, 0x007d72c4);
MGLOBAL(undefined1, DAT_007d72c8, 0x007d72c8);
MGLOBAL(undefined1, DAT_007d72cc, 0x007d72cc);
MGLOBAL(undefined1, DAT_007d72d0, 0x007d72d0);
MGLOBAL(undefined1, DAT_007d72f0, 0x007d72f0);
int FUN_004cbb60(void);
MGLOBAL(int, _DAT_007d738c, 0x007d738c);
MGLOBAL(int, _DAT_007d7390, 0x007d7390);
MGLOBAL(int, _DAT_007d7398, 0x007d7398);
MGLOBAL(int, _DAT_007d739c, 0x007d739c);
MGLOBAL(int, _DAT_007d7374, 0x007d7374);
MGLOBAL(float, _DAT_006181ac, 0x006181ac);
MGLOBAL(int, _DAT_007d7308, 0x007d7308);
MGLOBAL(float, _DAT_007d730c, 0x007d730c);
MGLOBAL(float, _DAT_007d7310, 0x007d7310);
MGLOBAL(float, _DAT_007d7314, 0x007d7314);
MGLOBAL(float, _DAT_007d7318, 0x007d7318);
MGLOBAL(float, _DAT_007d7348, 0x007d7348);
MGLOBAL(float, _DAT_007d734c, 0x007d734c);
MGLOBAL(float, _DAT_007d7350, 0x007d7350);
MGLOBAL(float, _DAT_007d731c, 0x007d731c);
MGLOBAL(float, _DAT_007d7320, 0x007d7320);
MGLOBAL(float, _DAT_007d7324, 0x007d7324);
MGLOBAL(float, _DAT_007d7328, 0x007d7328);
MGLOBAL(float, _DAT_007d732c, 0x007d732c);
MGLOBAL(float, _DAT_007d7330, 0x007d7330);
MGLOBAL(float, _DAT_007d7334, 0x007d7334);
MGLOBAL(float, _DAT_007d7338, 0x007d7338);
MGLOBAL(float, _DAT_007d733c, 0x007d733c);
MGLOBAL(float, _DAT_007d7340, 0x007d7340);
MGLOBAL(float, _DAT_007d7344, 0x007d7344);
MGLOBAL(float, _DAT_007d7354, 0x007d7354);
MGLOBAL(float, _DAT_007d7358, 0x007d7358);
MGLOBAL(float, _DAT_007d735c, 0x007d735c);
MGLOBAL(float, _DAT_007d7360, 0x007d7360);
MGLOBAL(float, _DAT_007d7364, 0x007d7364);
MGLOBAL(float, _DAT_007d7368, 0x007d7368);
MGLOBAL(float, _DAT_007d736c, 0x007d736c);
MGLOBAL(float, _DAT_007d72a4, 0x007d72a4);
MGLOBAL(float, _DAT_007d72a8, 0x007d72a8);
MGLOBAL(float, _DAT_007d72ac, 0x007d72ac);
MGLOBAL(float, _DAT_007d72b0, 0x007d72b0);
MGLOBAL(float, _DAT_007d72b4, 0x007d72b4);
MGLOBAL(float, _DAT_007d72b8, 0x007d72b8);
MGLOBAL(float, _DAT_007d72bc, 0x007d72bc);
MGLOBAL(float, _DAT_007d72c0, 0x007d72c0);
MGLOBAL(float, _DAT_007d72c4, 0x007d72c4);
MGLOBAL(float, _DAT_007d72c8, 0x007d72c8);
MGLOBAL(float, _DAT_007d72cc, 0x007d72cc);
MGLOBAL(float, _DAT_007d72d0, 0x007d72d0);
MGLOBAL(float, _DAT_007d72f0, 0x007d72f0);
// MATCH: 0x004f46a0 FUN_004f46a0
/* [C1 2026-05-20] Signature: `u32 FUN_004f46a0(void)`. */

undefined4 FUN_004f46a0(void)

{
  int iVar1;
  
  if (DAT_007d7384 != 0) {
    (**(code **)(DAT_007d3ff8 + 0x10c))(DAT_007d7384);
    DAT_007d7384 = 0;
  }
  DAT_007d7378 = 0;
  DAT_007d737c = 0;
  DAT_007d7380 = 0;
  if (DAT_007d7388 != 0) {
    (**(code **)(DAT_007d3ff8 + 0x10c))(DAT_007d7388);
    DAT_007d7388 = 0;
  }
  _DAT_007d738c = 0;
  _DAT_007d7390 = 0;
  if (DAT_007d7394 != 0) {
    (**(code **)(DAT_007d3ff8 + 0x10c))(DAT_007d7394);
    DAT_007d7394 = 0;
  }
  _DAT_007d7398 = 0;
  _DAT_007d739c = 0;
  iVar1 = FUN_004cbb60();
  _DAT_007d7374 = *(int *)(iVar1 + 0xa0);
  if (_DAT_007d7374 == 0) {
    _DAT_007d7374 = -1;
  }
  DAT_006181a0 = 0.0f;
  DAT_006181a4 = 0.0f;
  DAT_006181a8 = 0.0f;
  _DAT_006181ac = 1.0f;
  _DAT_007d7308 = 3;
  _DAT_007d730c = 0.0f;
  _DAT_007d7310 = 0.0f;
  _DAT_007d7314 = 0.0f;
  _DAT_007d7318 = 1.0f;
  _DAT_007d7348 = 0.0f;
  _DAT_007d734c = 0.0f;
  _DAT_007d7350 = 0.0f;
  _DAT_007d731c = 0.0f;
  _DAT_007d7320 = 0.0f;
  _DAT_007d7324 = 0.0f;
  _DAT_007d7328 = 1.0f;
  _DAT_007d732c = 0.0f;
  _DAT_007d7330 = 0.0f;
  _DAT_007d7334 = 0.0f;
  _DAT_007d7338 = 1.0f;
  _DAT_007d733c = 0.0f;
  _DAT_007d7340 = 0.0f;
  _DAT_007d7344 = 0.0f;
  _DAT_007d7354 = 0.0f;
  _DAT_007d7358 = 0.0f;
  _DAT_007d735c = 0.0f;
  _DAT_007d7360 = 0.0f;
  _DAT_007d7364 = 0.0f;
  _DAT_007d7368 = 0.0f;
  _DAT_007d736c = 0.0f;
  _DAT_007d72a4 = 0.0f;
  _DAT_007d72a8 = 0.0f;
  _DAT_007d72ac = 0.0f;
  _DAT_007d72b0 = 1.0f;
  _DAT_007d72b4 = 0.0f;
  _DAT_007d72b8 = 0.0f;
  _DAT_007d72bc = 0.0f;
  _DAT_007d72c0 = 1.0f;
  _DAT_007d72c4 = 0.0f;
  _DAT_007d72c8 = 0.0f;
  _DAT_007d72cc = 0.0f;
  _DAT_007d72d0 = 1.0f;
  _DAT_007d72f0 = 1.0f;
  return 1;
}
