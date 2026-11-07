// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004cc6e0 (h2): 45.9% (regs 54.1%). Raw 42.6%. Chunked write: memcpy intrinsic into a 0x100-byte stack
   buffer, then 0x004cbe80(stream, buf, n). Residual (a): with any local array above 64 bytes plus the memcpy
   intrinsic, 9176 emits an aligned ebp frame (push ebp / and esp,-8) that the original does not have. With char
   buf[0x40] every other instruction matches (72.0%, the only differences are the frame size). Tried:
   char/uint/short arrays, struct-wrapped buffer, block-scoped buffer, explicit copy loops (no rep movs), /G3 /G4
   /G5 /G6 /G7, no /G, /GS, /Gs. */
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
typedef unsigned int uint;
void *memcpy(void *, const void *, unsigned int);
#pragma intrinsic(memcpy)
int FUN_004cbe80(undefined4 p0, const void *p1, uint p2);
// MATCH: 0x004cc6e0 FUN_004cc6e0
undefined4 FUN_004cc6e0(undefined4 param_1,const char *param_2,uint param_3)
{
  uint n;
  char buf[0x100];
  while (param_3) {
    n = (param_3 < 0x100) ? param_3 : 0x100;
    memcpy(buf, param_2, n);
    if (!FUN_004cbe80(param_1, buf, n)) return 0;
    param_3 -= n;
    param_2 += n;
  }
  return param_1;
}
