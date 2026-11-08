// FLAGS: /O2 /Ob1 /G5 /Oy-
/* MATCH-S20 0x004cb490 (h1): 83.3% (regs 88.9%) with FLAGS /O2 /Ob1 /G5 /Oy- (55.6% under /O2 /Ob1 /G5).
   The original has an ebp frame (push ebp; mov ebp, esp) in an RW core where the neighbours have none, and saves both
   esi and edi in the prologue before the param_2 test; ours saves esi only on the copy path. That pair is the
   signature of an inline __asm body (cl saves every register the asm touches at entry and keeps a frame), which
   the brief excludes. Tried: struct copy through a selected pointer, two memcpy intrinsics (55.6%), /Oy- with the
   game flags (77.8%), /O1 /Oy- (68.4%). Class (c) [UNCERTAIN: no source witness of the asm]. With param_2 nonzero copies 64 bytes into it from the pointer
   DAT_007d4158[param_1], or from DAT_005d8b90 when that entry is 0. */
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
typedef struct Blk64 { int v[16]; } Blk64;
MGLOBALA(Blk64 *, DAT_007d4158, 0x104, 0x007d4158);
MGLOBAL(Blk64, DAT_005d8b90, 0x005d8b90);
void *memcpy(void *, const void *, unsigned int);
#pragma intrinsic(memcpy)

// MATCH: 0x004cb490 FUN_004cb490
void FUN_004cb490(int param_1, Blk64 *param_2)
{
  if (param_2 != 0) {
    if (DAT_007d4158[param_1] != 0) {
      memcpy(param_2, DAT_007d4158[param_1], sizeof(Blk64));
    } else {
      memcpy(param_2, &DAT_005d8b90, sizeof(Blk64));
    }
  }
}
