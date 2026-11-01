// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00444ff0: 100% (regs 100%) by hand (h8), raw 81.5%. Bytes identical incl. the 4-entry jump table
   (Ghidra size includes it). What moved it: colours as an RwRGBA struct passed by value (byte stores into one
   local, then push), coordinates as float literals, the four spline banks as Spline[3] arrays of 0x204-byte
   records, `DAT_007f1a90 & 3` with a signed `/ 4`, the last loop as two biased origins (-16 - (int)(v * -4.0f)),
   and the switch written case 0..3 plus a separate default with the same body as case 0 (a `case 0: default:`
   label pair gives a dec/je chain instead of the jump table). Strings are literals (relocheck byte-checks them). */
/* ---- MATCH-S08 preamble (same text in every re/match/s08 source; not part of any function body) ----
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
typedef struct { unsigned char r; unsigned char g; unsigned char b; unsigned char a; } RwRGBA_00444ff0;
typedef struct { float x; float y; } SplinePt_00444ff0;
typedef struct { SplinePt_00444ff0 pt[64]; int n; } Spline_00444ff0;
typedef int (*SprintfFn_00444ff0)(char *, const char *, ...);
MGLOBAL(int, DAT_007f1a64, 0x007f1a64);
MGLOBAL(int, DAT_007f1a68, 0x007f1a68);
MGLOBAL(int, DAT_007f1a6c, 0x007f1a6c);
MGLOBAL(int, DAT_007f1a54, 0x007f1a54);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007f09e4, 0x007f09e4);
MGLOBAL(int, DAT_007f1a74, 0x007f1a74);
MGLOBAL(int, DAT_007f1a78, 0x007f1a78);
MGLOBAL(int, DAT_007f1a7c, 0x007f1a7c);
MGLOBAL(int, DAT_007f1a80, 0x007f1a80);
MGLOBAL(int, DAT_007f1a84, 0x007f1a84);
MGLOBAL(int, DAT_007f1a88, 0x007f1a88);
MGLOBAL(int, DAT_007f1a8c, 0x007f1a8c);
MGLOBAL(int, DAT_007f1a90, 0x007f1a90);
MGLOBAL(int, DAT_007f1a94, 0x007f1a94);
MGLOBAL(int, DAT_007f1a98, 0x007f1a98);
MGLOBAL(int, DAT_00801a9c, 0x00801a9c);
MGLOBAL(float, DAT_007f1a58, 0x007f1a58);
MGLOBAL(float, DAT_007f1a5c, 0x007f1a5c);
MGLOBALA(Spline_00444ff0, DAT_00801aa0, 3, 0x00801aa0);
MGLOBALA(Spline_00444ff0, DAT_008020ac, 3, 0x008020ac);
MGLOBALA(Spline_00444ff0, DAT_008026b8, 3, 0x008026b8);
MGLOBALA(Spline_00444ff0, DAT_00802cc4, 3, 0x00802cc4);
float FUN_00408a50(int p0);
char FUN_004173e0(int x, int y);
void FUN_004430e0(float x0, float y0, float x1, float y1, float x2, float y2, RwRGBA_00444ff0 col);
void FUN_00443210(float x, float y, float v);
void FUN_00444b60(Spline_00444ff0 *s);
int FUN_005586f0(int p0, char *text, int x, int y, int p4);
// MATCH: 0x00444ff0 FUN_00444ff0
void FUN_00444ff0(void)
{
  RwRGBA_00444ff0 col;
  int sel;
  int i;
  int j;
  int ox;
  int oy;
  char c;
  float y0;
  float y1;
  float x0;
  float x1;
  float fy;
  Spline_00444ff0 *s;
  char buf[256];

  sel = DAT_007f1a64;
  switch (DAT_007f1a68) {
  case 0:
    s = &DAT_00801aa0[DAT_007f1a6c];
    break;
  case 1:
    s = &DAT_008020ac[DAT_007f1a6c];
    break;
  case 2:
    s = &DAT_008026b8[DAT_007f1a6c];
    break;
  case 3:
    s = &DAT_00802cc4[DAT_007f1a6c];
    break;
  default:
    s = &DAT_00801aa0[DAT_007f1a6c];
    break;
  }
  FUN_00444b60(s);
  col.r = 0x20;
  col.g = 0x20;
  col.b = 0x20;
  col.a = 0xff;
  FUN_004430e0(384.0f, 0.0f, 512.0f, 0.0f, 384.0f, 512.0f, col);
  FUN_004430e0(512.0f, 0.0f, 512.0f, 512.0f, 384.0f, 512.0f, col);
  col.a = 0xff;
  col.b = 0xff;
  col.g = 0xff;
  col.r = 0xff;
  FUN_004430e0(192.0f, 192.0f, 208.0f, 192.0f, 192.0f, 193.0f, col);
  FUN_004430e0(192.0f, 192.0f, 192.0f, 208.0f, 193.0f, 192.0f, col);
  FUN_004430e0(192.0f, 208.0f, 208.0f, 208.0f, 192.0f, 209.0f, col);
  FUN_004430e0(208.0f, 192.0f, 208.0f, 208.0f, 209.0f, 192.0f, col);
  col.b = 0x40;
  col.g = 0x40;
  col.r = 0x40;
  col.a = 0xff;
  y0 = (float)sel * 20.0f + 28.0f;
  y1 = y0 + 18.0f;
  FUN_004430e0(392.0f, y0, 504.0f, y0, 392.0f, y1, col);
  FUN_004430e0(392.0f, y1, 504.0f, y0, 504.0f, y1, col);
  if (DAT_007f1a54 == 0) {
    (*(SprintfFn_00444ff0 *)(DAT_007d3ff8 + 0xc4))(buf, "race lines");
    FUN_005586f0(DAT_007f09e4, buf, 400, 0x1e, 0);
    (*(SprintfFn_00444ff0 *)(DAT_007d3ff8 + 0xc4))(buf, "inside lines");
    FUN_005586f0(DAT_007f09e4, buf, 400, 0x32, 0);
    (*(SprintfFn_00444ff0 *)(DAT_007d3ff8 + 0xc4))(buf, "slow lines");
    FUN_005586f0(DAT_007f09e4, buf, 400, 0x46, 0);
    (*(SprintfFn_00444ff0 *)(DAT_007d3ff8 + 0xc4))(buf, "cheat lines");
    FUN_005586f0(DAT_007f09e4, buf, 400, 0x5a, 0);
    (*(SprintfFn_00444ff0 *)(DAT_007d3ff8 + 0xc4))(buf, "danger data");
    FUN_005586f0(DAT_007f09e4, buf, 400, 0x6e, 0);
    (*(SprintfFn_00444ff0 *)(DAT_007d3ff8 + 0xc4))(buf, "save data");
    FUN_005586f0(DAT_007f09e4, buf, 400, 0x82, 0);
    (*(SprintfFn_00444ff0 *)(DAT_007d3ff8 + 0xc4))(buf, "?????");
    FUN_005586f0(DAT_007f09e4, buf, 400, 0x96, 0);
    (*(SprintfFn_00444ff0 *)(DAT_007d3ff8 + 0xc4))(buf, "clear data");
    FUN_005586f0(DAT_007f09e4, buf, 400, 0xaa, 0);
    (*(SprintfFn_00444ff0 *)(DAT_007d3ff8 + 0xc4))(buf, "race");
    FUN_005586f0(DAT_007f09e4, buf, 400, 0xbe, 0);
  }
  if (DAT_007f1a54 == 1) {
    (*(SprintfFn_00444ff0 *)(DAT_007d3ff8 + 0xc4))(buf, "clear data?");
    FUN_005586f0(DAT_007f09e4, buf, 400, 0x1e, 0);
    (*(SprintfFn_00444ff0 *)(DAT_007d3ff8 + 0xc4))(buf, "no");
    FUN_005586f0(DAT_007f09e4, buf, 400, 0x46, 0);
    (*(SprintfFn_00444ff0 *)(DAT_007d3ff8 + 0xc4))(buf, "yes");
    FUN_005586f0(DAT_007f09e4, buf, 400, 0x5a, 0);
  }
  if (DAT_007f1a54 == 3) {
    (*(SprintfFn_00444ff0 *)(DAT_007d3ff8 + 0xc4))(buf, "save data?");
    FUN_005586f0(DAT_007f09e4, buf, 400, 0x1e, 0);
    (*(SprintfFn_00444ff0 *)(DAT_007d3ff8 + 0xc4))(buf, "no");
    FUN_005586f0(DAT_007f09e4, buf, 400, 0x46, 0);
    (*(SprintfFn_00444ff0 *)(DAT_007d3ff8 + 0xc4))(buf, "yes");
    FUN_005586f0(DAT_007f09e4, buf, 400, 0x5a, 0);
  }
  if (DAT_007f1a54 == 4) {
    (*(SprintfFn_00444ff0 *)(DAT_007d3ff8 + 0xc4))(buf, "race options");
    FUN_005586f0(DAT_007f09e4, buf, 400, 0x1e, 0);
    (*(SprintfFn_00444ff0 *)(DAT_007d3ff8 + 0xc4))(buf, "back");
    FUN_005586f0(DAT_007f09e4, buf, 400, 0x46, 0);
    (*(SprintfFn_00444ff0 *)(DAT_007d3ff8 + 0xc4))(buf, "reset car");
    FUN_005586f0(DAT_007f09e4, buf, 400, 0x5a, 0);
    if (DAT_007f1a74 == 0) {
      (*(SprintfFn_00444ff0 *)(DAT_007d3ff8 + 0xc4))(buf, "start race");
    } else {
      (*(SprintfFn_00444ff0 *)(DAT_007d3ff8 + 0xc4))(buf, "stop race");
    }
    FUN_005586f0(DAT_007f09e4, buf, 400, 0x6e, 0);
    (*(SprintfFn_00444ff0 *)(DAT_007d3ff8 + 0xc4))(buf, "single step");
    FUN_005586f0(DAT_007f09e4, buf, 400, 0x82, 0);
    (*(SprintfFn_00444ff0 *)(DAT_007d3ff8 + 0xc4))(buf, "Lap frames");
    FUN_005586f0(DAT_007f09e4, buf, 400, 0xbe, 0);
    (*(SprintfFn_00444ff0 *)(DAT_007d3ff8 + 0xc4))(buf, "%d", DAT_007f1a7c);
    FUN_005586f0(DAT_007f09e4, buf, 400, 0xd2, 0);
    (*(SprintfFn_00444ff0 *)(DAT_007d3ff8 + 0xc4))(buf, "%d", DAT_007f1a80);
    FUN_005586f0(DAT_007f09e4, buf, 400, 0xe6, 0);
    (*(SprintfFn_00444ff0 *)(DAT_007d3ff8 + 0xc4))(buf, "%d", DAT_007f1a84);
    FUN_005586f0(DAT_007f09e4, buf, 400, 0xfa, 0);
    x0 = FUN_00408a50(0);
    if (x0 > 0.5f && x0 < 0.7f) {
      if (DAT_007f1a88 == 0) {
        DAT_007f1a8c = DAT_007f1a78;
      }
      DAT_007f1a88 = 1;
      if (DAT_007f1a78 - DAT_007f1a8c > 0x3c) {
        DAT_007f1a84 = DAT_007f1a80;
        DAT_007f1a80 = DAT_007f1a7c;
        DAT_007f1a7c = DAT_007f1a78 - DAT_007f1a8c;
        DAT_007f1a88 = 0;
      }
    }
  }
  if (DAT_007f1a54 == 6) {
    (*(SprintfFn_00444ff0 *)(DAT_007d3ff8 + 0xc4))(buf, "Edit Spline %d", DAT_007f1a6c);
    FUN_005586f0(DAT_007f09e4, buf, 400, 0x1e, 0);
    (*(SprintfFn_00444ff0 *)(DAT_007d3ff8 + 0xc4))(buf, "Edit Point %d", DAT_007f1a98);
    FUN_005586f0(DAT_007f09e4, buf, 400, 0x46, 0);
    (*(SprintfFn_00444ff0 *)(DAT_007d3ff8 + 0xc4))(buf, "Insert Point");
    FUN_005586f0(DAT_007f09e4, buf, 400, 0x5a, 0);
    (*(SprintfFn_00444ff0 *)(DAT_007d3ff8 + 0xc4))(buf, "Delete Point");
    FUN_005586f0(DAT_007f09e4, buf, 400, 0x6e, 0);
    (*(SprintfFn_00444ff0 *)(DAT_007d3ff8 + 0xc4))(buf, "Back");
    FUN_005586f0(DAT_007f09e4, buf, 400, 0x82, 0);
  }
  (*(SprintfFn_00444ff0 *)(DAT_007d3ff8 + 0xc4))(buf, "memory=%d", DAT_00801a9c);
  FUN_005586f0(DAT_007f09e4, buf, 400, 0x1c2, 0);
  if (DAT_007f1a54 == 0 && DAT_007f1a64 == 4) {
    x0 = (float)(DAT_007f1a90 & 3) * 20.0f;
    fy = (float)(DAT_007f1a90 / 4 - DAT_007f1a94);
    if (fy > 7.0f) {
      DAT_007f1a94 = DAT_007f1a94 + 1;
    } else if (fy < 0.0f) {
      DAT_007f1a94 = DAT_007f1a94 - 1;
    }
    x1 = x0 + 400.0f;
    x0 = x0 + 420.0f;
    FUN_004430e0(x1, 250.0f, x0, 250.0f, x1, 270.0f, col);
    FUN_004430e0(x0, 250.0f, x0, 270.0f, x1, 270.0f, col);
    for (i = 0; i < 4; i++) {
      FUN_00443210((float)i * 20.0f + 410.0f, 260.0f, (float)(i + DAT_007f1a94 * 4));
    }
  }
  ox = -16 - (int)(DAT_007f1a5c * -4.0f);
  oy = -16 - (int)(DAT_007f1a58 * -4.0f);
  for (j = 0; j < 0x20; j++) {
    fy = (float)j * 16.0f;
    for (i = 0; i < 0x18; i++) {
      c = FUN_004173e0(ox + i - 16, oy + 16 - j);
      if (c != -1) {
        FUN_00443210((float)i * 16.0f + 8.0f, fy + 8.0f, (float)c);
      }
    }
  }
}
