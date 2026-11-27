// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* RACE-LOAD-004E99B0 (run 57, 2026-11-27): 0x0057c4b0 (70 B, no source before), written from the disassembly of
   0x0057c4b0..0x0057c4f6 (Ghidra pool slot Mashed_pool0, read-only). Called by 0x0047f4c0 on the race load. Adds to the
   record p: +0x04 += 1, +0x08 += a, +0x10 += b, +0x14 += b, +0x1c += b * 5, +0x20 += b * 5, and stores b to DAT_00624044
   (0x0057c4ef). hooks.csv files it under third-party-library[qhull-2002.1]; no library identity checked here. */
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
MGLOBAL(int, DAT_00624044, 0x00624044);

// MATCH: 0x0057c4b0 FUN_0057c4b0
void FUN_0057c4b0(int p, int a, int b)
{
    *(int *)(p + 0x4) = *(int *)(p + 0x4) + 1;
    *(int *)(p + 0x8) = *(int *)(p + 0x8) + a;
    *(int *)(p + 0x10) = *(int *)(p + 0x10) + b;
    *(int *)(p + 0x14) = *(int *)(p + 0x14) + b;
    *(int *)(p + 0x1c) = *(int *)(p + 0x1c) + b * 5;
    *(int *)(p + 0x20) = *(int *)(p + 0x20) + b * 5;
    DAT_00624044 = b;
}
