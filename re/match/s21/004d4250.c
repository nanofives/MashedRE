// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d4250 (h2): 75.0% (regs 87.5%, ops 87.5%). Top-down stack allocator: size rounded to 4,
   DAT_007d4714 -= size; below DAT_007d4718 -> undo, RwError {1, FUN_004d7ff0(0x80000013, size)} through
   FUN_004d8480, return NULL; else return the new top. FUN_004d7ff0 is variadic cdecl (one `add esp,0xc` covers
   both calls), not the 1-argument prototype Ghidra shows.
   Residual (2 spots, registers + scheduling only): the original loads the argument into eax and does
   `lea ecx,[eax+3]` with the DAT_007d4718 load and `sub esp,8` hoisted above it, and takes &err in ecx after the
   errorCode store; ours does `mov ecx,[esp+4]; add ecx,3` and takes &err in eax. Tried 5 forms (param rewrite,
   separate aligned/top locals, err at function scope, int/unsigned) and the permuter (99 compiles, no gain).
   Class (b): register choice not moved by any form found. */
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
typedef struct RwError { int pluginID; int errorCode; } RwError;
MGLOBAL(unsigned int, DAT_007d4718, 0x007d4718);
MGLOBAL(unsigned int, DAT_007d4714, 0x007d4714);
int FUN_004d7ff0(int code, ...);
RwError *FUN_004d8480(RwError *err);
// MATCH: 0x004d4250 FUN_004d4250
void *FUN_004d4250(int size)
{
  unsigned int sz = (size + 3) & ~3;
  DAT_007d4714 -= sz;
  if (DAT_007d4714 < DAT_007d4718) {
    RwError err;
    DAT_007d4714 += sz;
    err.pluginID = 1;
    err.errorCode = FUN_004d7ff0(0x80000013, sz);
    FUN_004d8480(&err);
    return 0;
  }
  return (void *)DAT_007d4714;
}
