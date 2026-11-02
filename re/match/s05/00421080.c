// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00421080 (34 B Ghidra size, 43 B obj symbol): 80.0% by instructions because Ghidra's size stops at
   0x004210a2 (before the final store + ret); `match.py --bytes`: whole obj symbol BYTE-IDENTICAL (relocations
   masked), so matched. By hand. DAT_0063e588 = 128 smoke records of 0x2c bytes, count at 0x0063fb88 right after
   (layout from 0x00421160 / 0x004212b0). Needs an int-index loop: the pointer loop gives jb, the original is jl.
   Residual: none. Relocheck OK. */
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
typedef struct Smoke0063e588 {
  float sx;            /* +0x00 */
  float sy;            /* +0x04 */
  float pos[3];        /* +0x08 */
  float ang;           /* +0x14 */
  float spin;          /* +0x18 */
  float age;           /* +0x1c, -1.0f = free */
  float f20;           /* +0x20 */
  unsigned char col[4];/* +0x24 */
  int owner;           /* +0x28 */
} Smoke0063e588;
MGLOBALA(Smoke0063e588, DAT_0063e588, 128, 0x0063e588);
MGLOBAL(int, DAT_0063fb88, 0x0063fb88);
// MATCH: 0x00421080 FUN_00421080
void FUN_00421080(void)
{
  int i;
  for (i = 0; i < 128; i++)
    DAT_0063e588[i].age = -1.0f;
  DAT_0063fb88 = 0;
}
