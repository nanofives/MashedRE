// FLAGS: /O2 /Ob1 /G5
// h2 hand-matched. 67.9% (regs-renamed 68.4%) via match_permute (statement moves in the two light/camera list-removal loops; 1232 compiles). Raw Ghidra form 53.7%. RW clump destroy with inlined atomic destroy. Residual: original hoists xor ebp,ebp, keeps the clump in esi, and caches the link-offset global in ecx across the removal stores. Class (b).
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
typedef unsigned int undefined4;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d7200, 0x007d7200);
MGLOBAL(int, DAT_007d7230, 0x007d7230);
MGLOBAL(int, DAT_007d722c, 0x007d722c);
MGLOBAL(undefined4, DAT_00618664, 0x00618664);
MGLOBAL(undefined4, DAT_0061867c, 0x0061867c);
void FUN_004c0790(void *p0);
undefined4 FUN_004c0de0(int p0);
void FUN_004c0e50(int p0);
undefined4 FUN_004c1cf0(undefined4 p0);
int FUN_004d8060(void *p0, void *p1);
undefined4 FUN_004d8bd0(int * p0);
undefined4 FUN_004e4440(void *p0);
undefined4 FUN_004e4d90(undefined4 p0);
undefined4 FUN_004e8ea0(int p0);
// MATCH:
