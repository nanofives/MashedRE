// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 h4 0x0042e8b0: best 50.8% (regs 72.3%, ops 92.9%), 235/238 ins. Raw converter failed (C2120); by hand.
   Structure is complete: same frame minus one spill slot, same branch layout for the two alpha ramps, same
   calls. As in 0x00430b90, FUN_00474890 and FUN_00472c60 take two RwV2d by value (the `mov [slot],imm; mov
   reg,[slot]; push reg` quads); c2 rgb zeroed byte by byte before the table, c1 rgb = 0xff after it.
   Residual (b) register allocation: the original keeps t (the &0x1ff phase) in a stack slot ([esp+0x18], stored
   right after the and) and param_1 in memory (imul reg,[esp+0x78]), and holds pos.x / size.x / pos.y in
   edi / ebx / ebp across both FUN_00474890 calls; ours gives t to edi and param_1 to ebx and reloads pos/size.
   Tried: t copy variables, declaration order, separate pos/size pairs for the quads, unsigned t, u wrap as
   >= / & 0x1ff, permuter (6 rounds, 521 compiles, +0.9% only via a branch inversion and a statement move that
   changes behaviour, rejected). */
/* ---- MATCH-S07 preamble (same text in every re/match/s07 source; not part of any function body) ----
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
typedef struct RwRGBA { unsigned char red, green, blue, alpha; } RwRGBA;
typedef struct RwV2d { float x, y; } RwV2d;
MGLOBAL(float, DAT_007f1010, 0x007f1010);
MGLOBAL(int, DAT_0067ea58, 0x0067ea58);
MGLOBAL(int, DAT_0067f0bc, 0x0067f0bc);
MGLOBAL(int, DAT_008990e0, 0x008990e0);
void *FUN_0042e590(int a1);
void FUN_00472c60(RwV2d a1, RwV2d a2, RwRGBA a3);
void FUN_00474890(void *a1, RwV2d a2, RwV2d a3, RwRGBA a4, int a5, int a6, int a7);

// MATCH: 0x0042e8b0 FUN_0042e8b0
void FUN_0042e8b0(int param_1)
{
  RwRGBA c1, c2;
  int t, d, u, a, b, ta, tb;
  RwV2d pos, size;
  int tbl[16];

  c2.red = 0;
  c2.green = 0;
  c2.blue = 0;
  tbl[0] = 0; tbl[1] = 1; tbl[2] = 2; tbl[3] = 3; tbl[4] = 4; tbl[5] = 5;
  tbl[6] = 0; tbl[7] = 1; tbl[8] = 2; tbl[9] = 3; tbl[10] = 4; tbl[11] = 5;
  tbl[12] = 0; tbl[13] = 1; tbl[14] = 2; tbl[15] = 3;
  c1.red = 0xff;
  c1.green = 0xff;
  c1.blue = 0xff;
  pos.x = 330.0f; pos.y = 96.0f;
  size.x = 270.0f; size.y = 180.0f;
  t = (int)(DAT_007f1010 * 60.0f) & 0x1ff;
  if (t == 0x1e0)
    DAT_0067ea58 += 2;
  else if (t == 0xe0)
    DAT_0067f0bc += 2;
  if (!(DAT_0067f0bc & 1))
    DAT_0067f0bc++;
  if (t < 0xe0) {
    b = 0;
    a = 0xff;
  } else if (t < 0x100) {
    d = t - 0xe0;
    if (d < 0x10) {
      b = d * 16;
      a = 0xff;
    } else {
      a = (0x1f - d) * 16;
      b = 0xff;
    }
  } else if (t < 0x1e0) {
    a = 0;
    b = 0xff;
  } else {
    d = t - 0x1e0;
    if (d < 0x10) {
      b = 0xff;
      a = d * 16;
    } else {
      b = (0x1f - d) * 16;
      a = 0xff;
    }
  }
  ta = tbl[DAT_0067ea58 & 0xf];
  tb = tbl[DAT_0067f0bc & 0xf];
  if (a != 0) {
    c1.alpha = (unsigned char)(a * param_1 / 0xff);
    u = t + 0x20;
    if (u > 0x1ff)
      u -= 0x200;
    FUN_00474890(FUN_0042e590(0), pos, size, c1, (u << 8) / 0x120, ta, 0);
  }
  if (b != 0) {
    c1.alpha = (unsigned char)(b * param_1 / 0xff);
    FUN_00474890(FUN_0042e590(1), pos, size, c1, ((t - 0xe0) << 8) / 0x120, tb, 0);
  }
  c2.alpha = (unsigned char)param_1;
  pos.x = 328.0f; pos.y = 94.0f; size.x = 2.0f; size.y = 184.0f;
  FUN_00472c60(pos, size, c2);
  size.x = 274.0f; size.y = 2.0f;
  FUN_00472c60(pos, size, c2);
  pos.y = 276.0f;
  FUN_00472c60(pos, size, c2);
  pos.x = 600.0f; pos.y = 94.0f; size.x = 2.0f; size.y = 182.0f;
  FUN_00472c60(pos, size, c2);
  DAT_008990e0 += 0x20;
}
