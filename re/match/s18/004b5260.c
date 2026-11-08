// FLAGS: /O2 /GS /Ob1
/* MATCH-S18 0x004b5260 (h3): hand, 100%. RwRGBA-shaped colour (4 bytes) copied into +4 of every entry. */
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
typedef struct { unsigned char r, g, b, a; } RGBA_4b5260;
typedef struct { unsigned int f0; RGBA_4b5260 color; } Mat_4b5260;
int FUN_004b4140(void *p0);
void FUN_004b4130(void *p0, Mat_4b5260 **out);
// MATCH: 0x004b5260 FUN_004b5260
void FUN_004b5260(void *param_1, RGBA_4b5260 *param_2)
{
  RGBA_4b5260 white;
  Mat_4b5260 *list[16];
  int n;
  int i;

  white.r = 0xff;
  white.g = 0xff;
  white.b = 0xff;
  white.a = 0xff;
  n = FUN_004b4140(param_1);
  FUN_004b4130(param_1, list);
  if (param_2 == 0) {
    param_2 = &white;
  }
  for (i = 0; i < n; i++) {
    list[i]->color = *param_2;
  }
}
