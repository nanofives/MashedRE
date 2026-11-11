// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d42b0 (h1, by hand): best 77.8% exact, regs 88.9% (raw 55.6%). Bump allocator: size rounded to 4,
   DAT_007d4718 += size, over DAT_007d4714 -> undo + RWERROR(0x80000013, size) via _rwerror 0x004d7ff0 (varargs) and
   RwErrorSet 0x004d8480. Moves: varargs _rwerror + RwError local struct (55.6 -> 77.8). Residual: scheduling only. The
   original hoists the DAT_007d4714 load and `sub esp,8` above `lea ecx,[eax+3]` and takes &err in ecx before
   storing the _rwerror result. Tried: int/uint on param/size/globals (8 combos), file-static globals (3), a size
   temporary, compare spelled both ways, compound assign in the if, early-return form, permuter (92 compiles, no
   gain). Class (a). C++ ok. */
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
typedef struct { int pluginID; int errorCode; } RwError_;
MGLOBAL(uint, DAT_007d4714, 0x007d4714);
MGLOBAL(uint, DAT_007d4718, 0x007d4718);
int FUN_004d7ff0(int code, ...);
RwError_ *FUN_004d8480(RwError_ *err);
// MATCH: 0x004d42b0 FUN_004d42b0
uint FUN_004d42b0(uint param_1)

{
  RwError_ err;

  param_1 = (param_1 + 3) & ~3u;
  DAT_007d4718 += param_1;
  if (DAT_007d4718 > DAT_007d4714) {
    DAT_007d4718 -= param_1;
    err.pluginID = 1;
    err.errorCode = FUN_004d7ff0(0x80000013, param_1);
    FUN_004d8480(&err);
    return 0;
  }
  return DAT_007d4718 - param_1;
}
