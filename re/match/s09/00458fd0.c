// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00458fd0: 100% by hand (raw 83.3%). FUN_00458e00 takes 2 args (Ghidra gave it 3): the third
   push is FUN_00458fa0's second argument, pushed first by right-to-left evaluation. */
/* ---- MATCH-S09 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
typedef struct V3_00458fd0 {
  float x;
  float y;
  float z;
} V3_00458fd0;
int FUN_00458e00(V3_00458fd0 *p0, int p1);
int FUN_00458fa0(int p0, float p1);
// MATCH: 0x00458fd0 FUN_00458fd0
void FUN_00458fd0(V3_00458fd0 *p, int type, float v)
{
  FUN_00458fa0(FUN_00458e00(p, type), v);
}
