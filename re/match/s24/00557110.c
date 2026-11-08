// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x00557110 (h2): 96.5% (regs 96.5%), 57/57 instructions. Struct form (two V2d points and two
   V2d deltas at +0x10/+0x28/+0x40/+0x58, flags at +0x64) took the raw converter's 75.4% to 96.5%. The one
   residual: the original schedules the first compare (fld [ecx+0x28]; fcomp) before the integer copy of the
   spilled +0x5c delta (mov eax,[esp+0x14]; mov [ecx+0x5c],eax); ours interleaves it after. Tried: nested ifs,
   negated ||-form, a local for the +0x5c delta, permuter (123 compiles, no gain), flags /O2 /GS /Ob1 77.2%,
   /O1 and /O1 /Oy- 50.9%. Class (a): x87/integer scheduling choice no source form moved. */
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
typedef struct { float x, y; } V2d;
typedef struct {
  char pad0[0x10];
  V2d p0;            /* +0x10 */
  char pad1[0x10];
  V2d d0;            /* +0x28 */
  char pad2[0x10];
  V2d p1;            /* +0x40 */
  char pad3[0x10];
  V2d d1;            /* +0x58 */
  int pad4;
  unsigned int flags; /* +0x64 */
} Line2;
MGLOBAL(float, DAT_005d757c, 0x005d757c);
// MATCH: 0x00557110 FUN_00557110
Line2 *FUN_00557110(Line2 *param_1, V2d *param_2, V2d *param_3, V2d *param_4, V2d *param_5)
{
  param_1->p0 = *param_5;
  param_1->d0.x = param_4->x - param_5->x;
  param_1->d0.y = param_4->y - param_5->y;
  param_1->p1 = *param_2;
  param_1->d1.x = param_3->x - param_2->x;
  param_1->d1.y = param_3->y - param_2->y;
  if (param_1->d0.x == DAT_005d757c && param_1->d0.y == DAT_005d757c &&
      param_1->d1.x == DAT_005d757c && param_1->d1.y == DAT_005d757c) {
    param_1->flags &= ~2;
    return param_1;
  }
  param_1->flags |= 2;
  return param_1;
}
