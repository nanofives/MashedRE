// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004c8740 (h1): 100% (regs 100%). Permuter moved the `windowed` computation below the four stores (98.4% before). D3D9 object (DAT_007d4108) vtable +0x2c (__stdcall, 7 args incl. this) probed
   for multisample levels 2..16 with (DAT_007d410c, DAT_006181b0, DAT_00911f90, ~DAT_00911f98 & 1, q, 0); the last
   success goes to DAT_006181b4. A second call with level 1 and &q as the out pointer sets DAT_006181bc = q + 1 and
   raises DAT_006181b4 to it (unsigned compare). Ghidra read the out value as DAT_006181b0 (stack aliasing).
   FLAGS /G5 (RW core). */
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
typedef int (__stdcall *D3DFn7)(void *self, unsigned int a, int b, int c, unsigned int d, unsigned int e, unsigned int *f);
MGLOBAL(unsigned int, DAT_00911f98, 0x00911f98);
MGLOBAL(unsigned int, DAT_006181b4, 0x006181b4);
MGLOBAL(unsigned int, DAT_006181b8, 0x006181b8);
MGLOBAL(unsigned int, DAT_006181bc, 0x006181bc);
MGLOBAL(unsigned int, DAT_006181c0, 0x006181c0);
MGLOBAL(int **, DAT_007d4108, 0x007d4108);
MGLOBAL(int, DAT_00911f90, 0x00911f90);
MGLOBAL(int, DAT_006181b0, 0x006181b0);
MGLOBAL(unsigned int, DAT_007d410c, 0x007d410c);

// MATCH: 0x004c8740 FUN_004c8740
void FUN_004c8740(void)
{
  unsigned int windowed;
  unsigned int q;
  DAT_006181b4 = 1;
  DAT_006181b8 = 1;
  DAT_006181bc = 1;
  DAT_006181c0 = 1;
  windowed = ~DAT_00911f98 & 1;
  for (q = 2; q <= 16; q++) {
    if (((D3DFn7)(*DAT_007d4108)[0x2c / 4])(DAT_007d4108, DAT_007d410c, DAT_006181b0, DAT_00911f90, windowed, q, 0) >= 0) {
      DAT_006181b4 = q;
    }
  }
  if (((D3DFn7)(*DAT_007d4108)[0x2c / 4])(DAT_007d4108, DAT_007d410c, DAT_006181b0, DAT_00911f90, windowed, 1, &q) >= 0) {
    DAT_006181bc = q + 1;
    if (DAT_006181bc > DAT_006181b4) {
      DAT_006181b4 = DAT_006181bc;
    }
  }
}
