// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004cf160 (h4): 45.6% (regs 46.8%). RwImageSetGamma shape: stores gamma at module+0x20c, inverse =
   1.0f/gamma, entry 0 of both tables = 0, entries 1..255 = (int)(pow(i/255, invGamma or gamma) * 255 + 0.5).
   Residual: the original converts each float with an inline fnstcw / or ah,0xc / fldcw / fistp / fldcw sequence
   (0x004cf1f2..0x004cf20f, 0x004cf251..0x004cf26e) with stack temporaries; that is source-level inline asm
   (RwInt32FromReal style), cl 13.00 emits call __ftol for every C cast tried ((int), float temp, /QIfist, /Op).
   Class (c): __asm is not allowed in these sources. The rest of the code (table stores, pow args) lines up. */
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
typedef struct ImageGlobals {
  int pad[3]; unsigned char gammaTable[256]; unsigned char invGammaTable[256]; float gammaVal;
} ImageGlobals;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d4628, 0x007d4628);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(float, DAT_005ceb90, 0x005ceb90);
MGLOBAL(float, DAT_005cd04c, 0x005cd04c);
MGLOBAL(float, DAT_005cc32c, 0x005cc32c);
double FUN_004a2d30(double x, double y);
#define IMGGLOBAL(v) (((ImageGlobals *)(DAT_007d3ff8 + DAT_007d4628))->v)

// MATCH: 0x004cf160 FUN_004cf160
int FUN_004cf160(float gammaValue)
{
  int i;
  float invGamma;
  IMGGLOBAL(gammaVal) = gammaValue;
  invGamma = DAT_005cc320 / gammaValue;
  IMGGLOBAL(gammaTable)[0] = 0;
  IMGGLOBAL(invGammaTable)[0] = 0;
  for (i = 1; i < 256; i++) {
    float r = (float)i * DAT_005ceb90;
    float t;
    t = (float)FUN_004a2d30(r, invGamma) * DAT_005cd04c + DAT_005cc32c;
    IMGGLOBAL(gammaTable)[i] = (unsigned char)(int)t;
    t = (float)FUN_004a2d30(r, gammaValue) * DAT_005cd04c + DAT_005cc32c;
    IMGGLOBAL(invGammaTable)[i] = (unsigned char)(int)t;
  }
  return 1;
}
