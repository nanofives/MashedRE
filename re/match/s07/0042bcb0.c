// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x0042bcb0 (h6, by hand): best 84.9% (regs 86.0%). Raw failed to compile (C2067). Written from the disassembly: two V2 corners from x/640 and y/480 plus 0.04/0.03, char name[16] = "" (byte + 3 dwords + word + byte init), intrinsic strcpy "keyboard"/"joypad" by the mode word at 0x007e96fc[player*0x80], strcat "i" when suffix, texture lookup 0x004c5c00, draw 0x004b5750 with &col (RGBA passed by value). The float constants are literals (0.0015625f, 0.0020833334f, 0.04f, 0.03f = the .rdata values at 0x005cd5a8/0x005cc560/0x005cd18c/0x005cc724): with MGLOBAL floats cl loads the constant first (fld K; fmul x, 74.4%), with literals it is fld x; fmul K (79.1%); scaling x and y in place first gave the fld st(1) schedule (84.9%). Tried: switch on the mode (79.3%), mode in a local early or late, name declared first, raster init placement, fx/fy locals, unsigned player, {0} init, shifted mode pointer (all 84.9%), per-byte zeroing (34%), permuter 159 compiles (no gain). Residual class (b): integer scheduling in the prologue: the original zeroes name with ecx and indexes the mode table with eax (player loaded after the cookie store, name[0]/[13..15] stores after push esi); ours uses eax for zero and ecx for player, all init stores before push esi. relocheck BAD only because the instruction offsets differ (not 100%). */
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
typedef struct { float x; float y; } V2;
typedef struct { unsigned char r; unsigned char g; unsigned char b; unsigned char a; } RGBA4;
char *strcpy(char *d, const char *s);
char *strcat(char *d, const char *s);
#pragma intrinsic(strcpy, strcat)
MGLOBAL(float, DAT_005cd5a8, 0x005cd5a8);
MGLOBAL(float, DAT_005cc560, 0x005cc560);
MGLOBAL(float, DAT_005cd18c, 0x005cd18c);
MGLOBAL(float, DAT_005cc724, 0x005cc724);
MGLOBAL(int, DAT_00636ac8, 0x00636ac8);
MGLOBAL(int *, DAT_007d3ff8, 0x007d3ff8);
MGLOBALA(int, DAT_007e96fc, 0x200, 0x007e96fc);
void FUN_004b5750(int p0, V2 *p1, V2 *p2, RGBA4 *p3, int p4);
int *FUN_004c5c00(int p0, char *p1);
// MATCH: 0x0042bcb0 FUN_0042bcb0
void FUN_0042bcb0(int player, float x, float y, RGBA4 col, int suffix)
{
  V2 a;
  V2 b;
  char name[16] = "";
  int *tex;
  int raster;
  x = x * 0.0015625f;
  y = y * 0.0020833334f;
  a.x = x + 0.04f;
  a.y = 0.03f + y;
  b.x = x - 0.04f;
  b.y = y - 0.03f;
  raster = 0;
  if (DAT_007e96fc[player * 0x80] == 2) {
    strcpy(name, "keyboard");
  }
  else if (DAT_007e96fc[player * 0x80] == 1) {
    strcpy(name, "joypad");
  }
  if (suffix != 0) {
    strcat(name, "i");
  }
  tex = FUN_004c5c00(DAT_00636ac8, name);
  if (tex != 0) {
    raster = *tex;
  }
  FUN_004b5750(*DAT_007d3ff8, &b, &a, &col, raster);
}
