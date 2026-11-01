// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x004223b0: by hand, 100% (regs 100%), no residual.
   Box sup = -FLT_MAX (0xff7fffff), inf = FLT_MAX (0x7f7fffff), then
   RpClumpForAllAtomics (0x004e66d0) with the callback 0x004222f0. */
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
typedef struct BBox004223b0 { float sx, sy, sz, ix, iy, iz; } BBox004223b0;
typedef int (*AtomicCb004223b0)(int atomic, void *data);
int FUN_004222f0(int atomic, void *data);
int FUN_004e66d0(int clump, AtomicCb004223b0 cb, void *data);

// MATCH: 0x004223b0 _FUN_004223b0
void FUN_004223b0(BBox004223b0 *box, int clump)
{
    box->sz = -3.402823466e+38f;
    box->sy = -3.402823466e+38f;
    box->sx = -3.402823466e+38f;
    box->iz = 3.402823466e+38f;
    box->iy = 3.402823466e+38f;
    box->ix = 3.402823466e+38f;
    FUN_004e66d0(clump, FUN_004222f0, box);
}
