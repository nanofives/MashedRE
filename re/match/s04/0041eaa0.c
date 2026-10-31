// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041eaa0: 100% (regs 100%), raw g2s 91.4%. By hand. The fill loop over DAT_0063d840[4] must be an int-index loop: the pointer form (p < DAT_0063d840 + 4) gave 98.6% with jb instead of jl. The index loop is strength-reduced to the same pointer walk with a signed compare. The two 0.75f locals are a float[2] passed to FUN_004c1c80. Residual: none. */
/* ---- MATCH-S04 preamble (same text in every re/match/s04 source; not part of any function body) ----
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
typedef struct Tex_0041eaa0 { int v[16]; } Tex_0041eaa0; /* 0x40-byte objects at 0x0063d860 / 0x0063d8a0 */
MGLOBALA(void *, DAT_0063d840, 4, 0x0063d840);
MGLOBAL(Tex_0041eaa0, DAT_0063d8a0, 0x0063d8a0);
MGLOBAL(Tex_0041eaa0, DAT_0063d860, 0x0063d860);
MGLOBAL(Tex_0041eaa0 *, DAT_0063e49c, 0x0063e49c);
MGLOBAL(Tex_0041eaa0 *, DAT_0063e4a0, 0x0063e4a0);
MGLOBAL(int, DAT_00636acc, 0x00636acc);
MGLOBAL(void *, DAT_0063e490, 0x0063e490);
MGLOBAL(void *, DAT_0063e494, 0x0063e494);
MGLOBAL(int, DAT_0063d854, 0x0063d854);
MGLOBAL(int, DAT_0063d858, 0x0063d858);
MGLOBALA(char, s_beam_005cd268, 5, 0x005cd268);
MGLOBALA(char, s_car_shadow_005cd25c, 11, 0x005cd25c);
void *FUN_004c0b30(void);
int FUN_0040bb30(char *name);
void FUN_004770c0(Tex_0041eaa0 *t, int a, int b, int c);
void FUN_00476cb0(Tex_0041eaa0 *t, int a, int b);
int FUN_004b48e0(int w, int h, void **out, int *out2);
int FUN_004c1c80(void *raster, float *v);
int FUN_004c1c10(void *raster, int mode);
// MATCH: 0x0041eaa0 FUN_0041eaa0
void FUN_0041eaa0(void)
{
  int i;
  float uv[2];
  for (i = 0; i < 4; i++) {
    DAT_0063d840[i] = FUN_004c0b30();
  }
  DAT_0063e49c = &DAT_0063d8a0;
  FUN_004770c0(DAT_0063e49c, 0x80c, 4, FUN_0040bb30(s_beam_005cd268));
  FUN_00476cb0(DAT_0063e49c, 5, 2);
  DAT_0063e4a0 = &DAT_0063d860;
  FUN_004770c0(DAT_0063e4a0, 0x80c, 4, FUN_0040bb30(s_car_shadow_005cd25c));
  if (DAT_00636acc == 0) {
    uv[0] = 0.75f;
    uv[1] = 0.75f;
    FUN_004b48e0(0x100, 0x100, &DAT_0063e490, &DAT_0063d854);
    FUN_004c1c80(DAT_0063e490, uv);
    FUN_004c1c10(DAT_0063e490, 2);
    FUN_004b48e0(0x40, 0x40, &DAT_0063e494, &DAT_0063d858);
    FUN_004c1c80(DAT_0063e494, uv);
    FUN_004c1c10(DAT_0063e494, 2);
  }
}
