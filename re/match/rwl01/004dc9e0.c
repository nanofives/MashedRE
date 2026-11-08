// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004dc9e0: 100% regs 100%. Three arrays of 4 dwords; flag branch first (loop), single-slot path second, tail zeroing is a for-loop (4..0xc step 4) the compiler unrolls. class -. */
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
MGLOBAL(uint, DAT_00911fbc, 0x00911fbc);
MGLOBAL(undefined4, DAT_007d70c8, 0x007d70c8);
MGLOBALA(undefined4, DAT_007d70cc, 4, 0x007d70cc);
MGLOBALA(undefined4, DAT_007d70dc, 4, 0x007d70dc);
MGLOBALA(undefined4, DAT_007d70ec, 4, 0x007d70ec);
undefined4 FUN_004dcaa0(int p0, undefined4 * p1);
// MATCH: 0x004dc9e0 FUN_004dc9e0
undefined4 FUN_004dc9e0(void)
{
  int iVar1;
  uint uVar2;

  DAT_007d70c8 = 0;
  if ((DAT_00911fbc & 0x10000) != 0) {
    uVar2 = 0;
    do {
      *(undefined4 *)((int)DAT_007d70cc + uVar2) = 0;
      *(undefined4 *)((int)DAT_007d70dc + uVar2) = 0x40000;
      iVar1 = FUN_004dcaa0(0x40000,(undefined4 *)((int)DAT_007d70ec + uVar2));
      if (iVar1 == 0) {
        return 0;
      }
      uVar2 = uVar2 + 4;
    } while (uVar2 < 0x10);
    return 1;
  }
  DAT_007d70cc[0] = 0;
  DAT_007d70dc[0] = 0x40000;
  iVar1 = FUN_004dcaa0(0x40000,&DAT_007d70ec[0]);
  if (iVar1 == 0) {
    return 0;
  }
  for (uVar2 = 4; uVar2 < 0x10; uVar2 += 4) {
    *(undefined4 *)((int)DAT_007d70cc + uVar2) = 0;
    *(undefined4 *)((int)DAT_007d70dc + uVar2) = 0;
    *(undefined4 *)((int)DAT_007d70ec + uVar2) = 0;
  }
  return 1;
}
