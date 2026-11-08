// FLAGS: /O2 /GS /Ob1
/* MATCH-S18 0x004b4150 (helper h1): 100% (regs 100%). Fix: the default local is a 4-byte RwRGBA-shaped struct initialized {0xff,0xff,0xff,0xff} (byte stores), param_2 a pointer to it, struct copy in a for loop. Residual: none. */
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
typedef struct { uchar red; uchar green; uchar blue; uchar alpha; } RwRGBA_;
int FUN_005493d0(int p0);
int FUN_00549610(int p0, uint p1);
int FUN_00549620(int p0);
// MATCH: 0x004b4150 FUN_004b4150
void FUN_004b4150(int param_1,RwRGBA_ *param_2)
{
  RwRGBA_ white = {0xff,0xff,0xff,0xff};
  int geo;
  RwRGBA_ *cols;
  int n;
  int i;

  geo = FUN_005493d0(param_1);
  cols = *(RwRGBA_ **)(geo + 8);
  n = *(int *)(geo + 0x30);
  FUN_00549610(geo,8);
  if (param_2 == 0) {
    param_2 = &white;
  }
  for (i = 0; i < n; i++) {
    cols[i] = *param_2;
  }
  FUN_00549620(geo);
}
