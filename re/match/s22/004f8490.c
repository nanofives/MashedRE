// FLAGS: /O2 /Ob1 /G5
/* MATCH-S22 0x004f8490 (h6): 100% (regs 100%) under /O2 /Ob1 /G5. Classifies a point list by its 2D bounds
   (FUN_004f8400) against three float thresholds and flag bits of the int at 0x0091208c; returns 1/4/9/0xb/0xe.
   What moved it: int return through one result variable set AFTER the bounds call (else esi), if/else-if single
   exit (the compiler duplicates the epilogue), flags global typed int (unsigned char cost the first dword load).
   Residual: none. */
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
typedef struct V2 { float x; float y; } V2;
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005cc33c, 0x005cc33c);
MGLOBAL(int, DAT_0091208c, 0x0091208c);
void FUN_004f8400(const V2 *pts, int n, V2 *mn, V2 *mx);
// MATCH: 0x004f8490 FUN_004f8490
int FUN_004f8490(const V2 *pts, int n)
{
  V2 mn;
  V2 mx;
  int r;
  FUN_004f8400(pts, n, &mn, &mx);
  r = 1;
  if (mx.x <= DAT_005cc320 && mx.y <= DAT_005cc320) {
    if (mn.x >= DAT_005d757c && mn.y >= DAT_005d757c) {
      if (DAT_0091208c & 0x10) {
        r = 0xb;
      } else if (DAT_0091208c & 0x80) {
        r = 0xe;
      } else {
        r = 4;
      }
    } else if (mn.x >= DAT_005cc33c && mn.y >= DAT_005cc33c) {
      if (DAT_0091208c & 4) {
        r = 9;
      } else if (DAT_0091208c & 0x80) {
        r = 0xe;
      }
    }
  }
  return r;
}
