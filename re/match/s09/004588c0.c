// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x004588c0: 100% by hand (raw 60.6%). FUN_00458630 takes 1 argument (add esp, 4), the other two
   pushes Ghidra gave it are FUN_00490020's args 6-7 (7 args, add esp, 0x1c). Colour = 4 byte stores (RGBA). */
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
typedef struct Rgba004588c0 {
  uchar r;
  uchar g;
  uchar b;
  uchar a;
} Rgba004588c0;
undefined4 FUN_00458630(undefined4 p0);
void FUN_00490020(void *p0, Rgba004588c0 *p1, undefined4 p2, undefined4 p3, undefined4 p4, float p5, undefined4 p6);
// MATCH: 0x004588c0 FUN_004588c0
void FUN_004588c0(int param_1)
{
  int i;
  Rgba004588c0 col;

  col.r = 0xff;
  col.g = 0;
  col.b = 0;
  col.a = 0xff;
  i = 10;
  do {
    FUN_00490020((void *)(param_1 + 0x38), &col, 0, 0, FUN_00458630(*(undefined4 *)(param_1 + 0x24)), 1.0f, 0);
    i--;
  } while (i != 0);
}
