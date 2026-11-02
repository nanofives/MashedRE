// FLAGS: /O2 /GS /Ob1
/* MATCH-S19 0x004c3910 (h3): best 83.0% (regs 87.2%, ops 95.7%). V3d normalize via RW's table inverse sqrt (RwSplitBits union with
   volatile int members gives the store/reload of the int view; table pointer at RwGlobals+DAT_007d3ffc+4, index
   value held in its own variable then += the exponent term). Dot written z*z + (x*x + y*y) for the y,x,z load order.
   RwGlobals order here is DAT_007d3ffc + DAT_007d3ff8 + 4 (the brief's order gives relocheck BAD in this function).
   Residual: the 3 x87 pops around `test ecx,ecx`, ff8/ffc register swap, and the scale stores (orig r*in.x then
   in.y*r, ours r first each time); operand-order and tail-form variants + permuter (138) no move. Class (a). */
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
typedef struct RwV3d { float x, y, z; } RwV3d;
typedef union RwSplitBits { float nReal; volatile int nInt; volatile unsigned int nUInt; } RwSplitBits;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d3ffc, 0x007d3ffc);

// MATCH: 0x004c3910 FUN_004c3910
float FUN_004c3910(RwV3d *out, const RwV3d *in)
{
  RwSplitBits u;
  float r;
  u.nReal = in->z * in->z + (in->x * in->x + in->y * in->y);
  if (u.nInt) {
    const unsigned int *tab = *(unsigned int **)(DAT_007d3ffc + DAT_007d3ff8 + 4);
    unsigned int idx;
    u.nUInt += 0x800;
    idx = tab[(u.nUInt >> 12) & 0xfff];
    idx += (~u.nUInt >> 1) & 0x3fc00000;
    u.nUInt = idx;
  }
  r = u.nReal;
  out->x = r * in->x;
  out->y = in->y * r;
  out->z = in->z * r;
  return r;
}
