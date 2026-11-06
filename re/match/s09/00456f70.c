// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00456f70: 100% by hand (h5), relocheck OK. Raw 21.4% (unaff_ESI). static with one int parameter; cl
   9176 passes it in ESI here (pilot finding 1). The real caller 0x00457010 (call at 0x00457066 after
   lea esi, [ecx - 0xff00]) is not in this file, so a stand-in caller sits below it (not the real TU). Moved the score:
   multiplying len in place in each branch (len *= k) gives fmul [mem] on the returned ST0; a selected factor
   (ternary or temp) loads the constant and keeps len live (86.4%). Constants: 0x005cc9dc 0.95f, 0x005cc320 1.0f,
   0x005ce4e0 0.025f, 0x005cd03c 0.0001f, 0x005ce4dc -0.0001f. */
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
typedef struct V3_00456f70 { float x, y, z; } V3_00456f70;
MGLOBAL(float, DAT_005cc9dc, 0x005cc9dc);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(float, DAT_005ce4e0, 0x005ce4e0);
MGLOBAL(float, DAT_005cd03c, 0x005cd03c);
MGLOBAL(float, DAT_005ce4dc, 0x005ce4dc);
float FUN_00472650(float lo, float hi);
int FUN_0046d660(V3_00456f70 *out, unsigned int car);
float FUN_004c3ac0(V3_00456f70 *v);
int FUN_0046d740(V3_00456f70 *v, unsigned int car);
void FUN_0046be50(unsigned int car, int kind, int amount, float f);

// MATCH: 0x00456f70 FUN_00456f70
static void FUN_00456f70(int car)
{
  float r;
  V3_00456f70 v;
  float len;

  r = FUN_00472650(0.0f, 1.0f);
  if (r > DAT_005cc9dc) {
    FUN_0046d660(&v, car);
    len = FUN_004c3ac0(&v);
    if (DAT_005cc320 - r > DAT_005ce4e0) {
      len = len * DAT_005cd03c;
    } else {
      len = len * DAT_005ce4dc;
    }
    v.y = len;
    v.x = 0;
    v.z = 0;
    FUN_0046d740(&v, car);
  }
  FUN_0046be50(car, 6, 100, 2400.0f);
}

/* stand-in caller (not the real TU): the real caller 0x00457010 passes car in ESI (lea esi, [ecx - 0xff00]) */
void FUN_00456f70_caller(int *p, int k)
{
  if (*p != 0) {
    FUN_00456f70(k - 0xff00);
  }
}
