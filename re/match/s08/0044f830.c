// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x0044f830: 100% by hand (raw 56.7%). switch (i) writes the id base into param_1's own stack slot
   (the original reuses the dead parameter, Ghidra shows the same), Fx[3][10] slots at 0x00895600, a 3-float
   struct copy of DAT_00894da0[i] per slot, FUN_004b3f90 takes 1 argument, FUN_0044ecc0() at the end becomes the
   tail jmp. */
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
typedef struct { float x, y, z; } V3_0044f830;
typedef struct {
  V3_0044f830 pos;  /* 0x00 */
  int h;            /* 0x0c */
  char *p10;        /* 0x10 */
  char *obj;        /* 0x14 */
  int id;           /* 0x18 */
} Fx_0044f830;
typedef Fx_0044f830 FxRow_0044f830[10];
MGLOBALA(FxRow_0044f830, DAT_00895600, 3, 0x00895600);
MGLOBALA(Fx_0044f830 *, DAT_00894e20, 200, 0x00894e20);
MGLOBALA(V3_0044f830, DAT_00894da0, 3, 0x00894da0);
void FUN_0044ecc0(void);
int FUN_0047d130(int p0);
int FUN_0047fad0(int p0, V3_0044f830 *p1, int p2, int p3, float p4, float p5, int p6);
int FUN_004b3f90(int p0);
void FUN_004b6520(void *p0, int p1);
void FUN_0055ae70(int p0, char *p1, int p2);
char *FUN_0057c210(int p0);
// MATCH: 0x0044f830 FUN_0044f830
void FUN_0044f830(int param_1)
{
  int h;
  int i;
  int j;
  int id;
  Fx_0044f830 *e;
  V3_0044f830 v;

  FUN_004b6520(DAT_00894e20, 800);
  h = FUN_004b3f90(*(int *)(param_1 + 0x101d8));
  for (i = 0; i < 3; i++) {
    switch (i) {
    case 0:
      param_1 = 2000;
      break;
    case 1:
      param_1 = 0x834;
      break;
    case 2:
      param_1 = 0x898;
      break;
    }
    for (j = 0; j < 10; j++) {
      e = &DAT_00895600[i][j];
      v = DAT_00894da0[i];
      id = FUN_0047fad0(h, &v, 0, 0, 1.0f, 25.0f, j + param_1);
      e->id = id;
      e->h = FUN_0047d130(id);
      e->p10 = FUN_0057c210(e->h);
      e->pos.x = 0.0f;
      e->pos.y = 10.0f;
      e->pos.z = 0.0f;
      FUN_0055ae70(*(int *)(e->p10 + 0x24), e->p10, 7);
      DAT_00894e20[id] = e;
    }
  }
  FUN_0044ecc0();
}
