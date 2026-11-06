// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00495350 (h2): 100% (regs 100%). Hand-written: RGBA local with an initializer (separate byte stores put bl in the first store), separate float sy/sx locals (stack order), signed (int) pointer compare for the 4-entry 0x4c-stride scan at 0x007f1502, for(;;) loop. FUN_00499710 takes one pushed 0 here (00499710.c declares it void). */
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
typedef unsigned long DWORD;
typedef struct V2 { float x, y; } V2;
typedef struct RGBA { unsigned char r, g, b, a; } RGBA;
MGLOBALA(char, DAT_007f1502, 0x130, 0x007f1502);
int FUN_004671a0(int p0);
void FUN_00492d20(void);
int FUN_00493f70(int p0);
void FUN_00493f80(V2 *p0, V2 *p1);
int FUN_00493fc0(float p0, float p1);
void FUN_00493fd0(int cam, V2 *p1, V2 *p2, int p3, int p4);
void FUN_00494460(int p0);
int FUN_00494480(int p0);
void FUN_00494a80(int p0, int p1, int p2);
int FUN_00499710(int p0);
void FUN_004c19f0(int p0);
int FUN_004c1a00(int p0);
void FUN_004c1bb0(int p0, RGBA *p1, int p2);
void FUN_004c1be0(int p0, int p1);
__declspec(dllimport) void __stdcall Sleep(DWORD p0);
// MATCH: 0x00495350 FUN_00495350
void FUN_00495350(void)
{
  int found;
  int cam;
  int n;
  char *p;
  RGBA col = {0, 0x80, 0x80, 0xff};
  float sy;
  float sx;
  V2 b;
  V2 a;
  V2 one;
  V2 zero;
  int modes[4];

  n = 0;
  modes[0] = 1;
  modes[1] = 2;
  modes[2] = 3;
  modes[3] = 4;
  FUN_00494a80(0, 1, 0);
  FUN_00493f80(&a, &b);
  for (;;) {
    found = 0;
    cam = FUN_004671a0(0);
    FUN_00492d20();
    if (FUN_00494480(0) != 0) {
      p = DAT_007f1502;
      do {
        if (p[-0x4c0] != 0 && *p == 0) {
          found = 1;
          break;
        }
        p += 0x4c;
      } while ((int)p < (int)(DAT_007f1502 + 0x130));
      FUN_004c1bb0(cam, &col, 1);
      if (FUN_004c1a00(cam) != 0) {
        zero.x = 0.0f;
        sx = a.x / b.x;
        zero.y = 0.0f;
        one.x = 1.0f;
        one.y = 1.0f;
        sy = a.y / b.y;
        FUN_00493fd0(cam, &zero, &one, 0, FUN_00493fc0(sx, sy));
        FUN_004c19f0(cam);
      }
    }
    FUN_004c1be0(cam, FUN_00499710(0));
    Sleep(5);
    if (FUN_00493f70(0) == 0 || found) {
      FUN_00494460(0);
      n++;
      if (n >= 4) {
        return;
      }
      FUN_00494a80(0, modes[n], 0);
      FUN_00493f80(&a, &b);
    }
  }
}
