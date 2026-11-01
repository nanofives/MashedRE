// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00425b10 (81 B), h9 by hand: 100.0% (regs 100.0%). Raw 10%: int stores reuse xor eax,eax, the
   original stores immediates. All eight globals typed float (stores of 0.0f) give mov [g], 0 each. */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
MGLOBAL(float, DAT_008992a0, 0x008992a0);
MGLOBAL(float, DAT_008992ec, 0x008992ec);
MGLOBAL(float, DAT_00899338, 0x00899338);
MGLOBAL(float, DAT_00899384, 0x00899384);
MGLOBAL(float, DAT_008993d0, 0x008993d0);
MGLOBAL(float, DAT_0089941c, 0x0089941c);
MGLOBAL(float, DAT_00899468, 0x00899468);
MGLOBAL(float, DAT_008994b4, 0x008994b4);
// MATCH: 0x00425b10 FUN_00425b10
void FUN_00425b10(void)

{
  DAT_008992a0 = 0.0f;
  DAT_008992ec = 0.0f;
  DAT_00899338 = 0.0f;
  DAT_00899384 = 0.0f;
  DAT_008993d0 = 0.0f;
  DAT_0089941c = 0.0f;
  DAT_00899468 = 0.0f;
  DAT_008994b4 = 0.0f;
  return;
}
