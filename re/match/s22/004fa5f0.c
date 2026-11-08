// FLAGS: /O2 /Ob1 /G5
/* MATCH-S22 0x004fa5f0 (h7): best 62.5% (regs 82.1%, ops 94.6%) under /O2 /Ob1 /G5 (raw converter 51.8%). Not zlib
   code (D3D device caps init: shader version word at 0x00912064), so the game-flag set is not needed: /O2 /GS /Ob1
   /G5 and /Ox give the same score, /O1 43.9%, /O2 /Ob1 (G6) 51.8%. What moved it: the 0x007d97b0 block as
   memset(.., 0, 0x100) (rep stosd), the tail (0x007d98bc clamp) written into each of the three version branches
   (the original lays it out after the first branch only, 51.8 -> 57.9), and a 64-placement sweep of the two
   stores DAT_007d98b0 = 0 and DAT_007d98b4 = &DAT_007d73b0 (57.9 -> 62.5). Permuter: 185 compiles, 0 gains.
   Residual class (b): the original stores 0x007d98b0 through the edx zero register before the memset and keeps
   ecx/eax the other way round in the setle select; ours keeps the stores later and uses a third tail copy. */
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
void *memset(void *d, int c, unsigned int n);
#pragma intrinsic(memset)
MGLOBALA(int, DAT_007d73b0, 5, 0x007d73b0);
MGLOBALA(int, DAT_007d97b0, 64, 0x007d97b0);
MGLOBAL(int, DAT_007d98b0, 0x007d98b0);
MGLOBAL(int *, DAT_007d98b4, 0x007d98b4);
MGLOBAL(int, DAT_007d98b8, 0x007d98b8);
MGLOBAL(unsigned int, DAT_007d98bc, 0x007d98bc);
MGLOBAL(unsigned int, DAT_00912064, 0x00912064);
MGLOBAL(unsigned int, DAT_00912068, 0x00912068);
MGLOBAL(unsigned int, DAT_0091208c, 0x0091208c);
MGLOBAL(int, DAT_0091209c, 0x0091209c);
// MATCH: 0x004fa5f0 FUN_004fa5f0
void FUN_004fa5f0(void)
{
  unsigned int v;

  DAT_007d98b4 = DAT_007d73b0;
  DAT_007d73b0[0] = -1;
  DAT_007d73b0[1] = -1;
  DAT_007d73b0[2] = -1;
  DAT_007d73b0[3] = -1;
  DAT_007d73b0[4] = -1;
  memset(DAT_007d97b0, 0, 0x100);
  DAT_007d97b0[1] = 2;
  DAT_007d97b0[2] = 2;
  DAT_007d97b0[6] = 2;
  DAT_007d97b0[7] = 2;
  DAT_007d97b0[9] = 2;
  v = DAT_00912064 & 0xffff;
  DAT_007d97b0[3] = 4;
  DAT_007d97b0[10] = 4;
  DAT_007d98b0 = 0;
  DAT_007d97b0[11] = 6;
  if (v >= 0x300) {
    DAT_007d98b8 = 0x19;
    DAT_007d98bc = DAT_00912068;
    if (DAT_00912068 >= 0x100) {
      DAT_007d98bc = 0x100;
    }
  } else if (v >= 0x200) {
    DAT_007d98b8 = (DAT_0091209c > 0) ? 0x19 : 0x14;
    DAT_007d98bc = DAT_00912068;
    if (DAT_00912068 >= 0x100) {
      DAT_007d98bc = 0x100;
    }
  } else if (v >= 0x100) {
    DAT_007d98b8 = 10;
    DAT_0091208c &= ~0x3e;
    DAT_007d98bc = DAT_00912068;
    if (DAT_00912068 >= 0x100) {
      DAT_007d98bc = 0x100;
    }
  } else {
    DAT_007d98b8 = 0;
  }
}
