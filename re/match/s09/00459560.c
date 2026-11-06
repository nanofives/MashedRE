// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00459560: 100% by hand (81 B whole symbol BYTE-IDENTICAL). Raw was 50.0%. What moved it: the
   0x58-byte slot array at 0x0068b9f8 (Ghidra's 0x0068b9fc is &slot[0].f04) indexed in a plain for loop, bytes
   at +0x34..+0x37, FUN_00474d60 with its 2 stack args. Residual: none. */
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
typedef struct {
  int idx;          /* +0x00 */
  int f04;          /* +0x04 */
  int f08;          /* +0x08 */
  int f0c;          /* +0x0c */
  int f10;          /* +0x10 */
  int f14[8];
  unsigned char b34[4]; /* +0x34 */
  int mat;          /* +0x38 */
  float f3c[6];
  int clump;        /* +0x54 */
} Slot459560;
MGLOBALA(Slot459560, DAT_0068b9f8, 4, 0x0068b9f8);
void FUN_00474d60(int clump, int a2);
void FUN_004b5320(int clump, int a2, int a3);
// MATCH: 0x00459560 FUN_00459560
void FUN_00459560(void)
{
  int i;

  for (i = 0; i < 4; i++) {
    DAT_0068b9f8[i].f10 = 0;
    DAT_0068b9f8[i].f04 = 0;
    DAT_0068b9f8[i].f08 = -1;
    DAT_0068b9f8[i].f0c = -1;
    FUN_00474d60(DAT_0068b9f8[i].clump, 0);
    DAT_0068b9f8[i].b34[0] = 0xff;
    DAT_0068b9f8[i].b34[1] = 0;
    DAT_0068b9f8[i].b34[2] = 0;
    DAT_0068b9f8[i].b34[3] = 0;
    FUN_004b5320(DAT_0068b9f8[i].clump, 0x40, 1);
  }
}
