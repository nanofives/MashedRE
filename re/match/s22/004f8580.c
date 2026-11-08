// FLAGS: /O2 /Ob1 /G5
/* MATCH-S22 0x004f8580 (h6): best 91.4% (regs 91.4%) under /O2 /Ob1 /G5. RW plugin attach: engine plugin 0x50f
   (FUN_004c2d90 = engine register, 4 args) then two object-plugin registers (FUN_004e8f50, FUN_004f0910, 5 args,
   offsets into 0x007d73a8 / 0x007d73ac). Callbacks typed as RW plugin ctor/dtor/copy pointers. Raw converter (bool)
   was ~50%. Residual: the compiler cross-jumps the second `return 0` onto the first one (`jl` back) where the
   original keeps two `xor eax,eax; ret` blocks. Tried: early returns, nested ifs (82.9), && chain (71.4), local
   status, ternary, three explicit early returns, permuter (only 3 variants, no gain), flagsweep (best /G5 91.4).
   Class (a): return-block tail merge, no source shape stopped it. */
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
MGLOBAL(int, DAT_007d73a8, 0x007d73a8);
MGLOBAL(int, DAT_007d73ac, 0x007d73ac);
void *FUN_004f8600(void *dst, const void *src, int offset, int size);
void *FUN_004f8620(void *instance, int offset, int size);
void *FUN_004f8630(void *instance, int offset, int size);
void *FUN_00561f40(void *object, int offset, int size);
int FUN_004c2d90(int size, unsigned int pluginID, void *(*initCB)(void *, int, int), void *(*termCB)(void *, int, int));
int FUN_004e8f50(int size, unsigned int pluginID, void *(*ctor)(void *, int, int), void *(*dtor)(void *, int, int),
                 void *(*copy)(void *, const void *, int, int));
int FUN_004f0910(int size, unsigned int pluginID, void *(*ctor)(void *, int, int), void *(*dtor)(void *, int, int),
                 void *(*copy)(void *, const void *, int, int));
// MATCH: 0x004f8580 FUN_004f8580
int FUN_004f8580(void)
{
  if (FUN_004c2d90(0, 0x50f, FUN_004f8620, FUN_004f8630) < 0) {
    return 0;
  }
  DAT_007d73a8 = FUN_004e8f50(4, 0x50f, FUN_00561f40, FUN_00561f40, FUN_004f8600);
  if (DAT_007d73a8 < 0) {
    return 0;
  }
  DAT_007d73ac = FUN_004f0910(4, 0x50f, FUN_00561f40, FUN_00561f40, FUN_004f8600);
  if (DAT_007d73ac < 0) {
    return 0;
  }
  return 1;
}
