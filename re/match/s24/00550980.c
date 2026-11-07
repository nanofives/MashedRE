// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x00550980 (h1): 100% (regs 100%). The `add eax,0x28; call [eax+0xc]` pair comes from taking the address
   of an embedded ops struct at +0x28 of the object at [p+0x38] into a local, then calling its +0xc slot. */
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
typedef struct FsOps { void *f0; void *f4; void *f8; uint (*read)(int fp, void *buf, uint n); } FsOps;
typedef struct FsDev { char pad[0x28]; FsOps ops; } FsDev;

// MATCH: 0x00550980 FUN_00550980
uint FUN_00550980(void *param_1, uint param_2, int param_3, int param_4)
{
  FsOps *ops = &(*(FsDev **)(param_4 + 0x38))->ops;
  return ops->read(param_4, param_1, param_2 * param_3) / param_2;
}
