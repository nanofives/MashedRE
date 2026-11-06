// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x00491590 (h5, by hand): 92.9% (regs 97.2%). Key moves: index form v[i] from a local copy of DAT_00771530 (70% -> 87%), the 0.45/0.6 factors are float literals (0x005ce010/0x005cc318 hold 0.45f/0.6f; as globals the operand order flips), counter store between dir.y and dir.z, table .x stored through DAT_0077152c[i] before p is taken. Permuter x2 (no legal gain; one variant used p before assignment). Residual (b): r is duplicated with fst instead of fld st(0)+fstp, and eax/ecx/edx naming in the a/-a pushes. */
/* ---- MATCH-S15 preamble (same text in every re/match/s15 source; not part of any function body) ----
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
typedef struct RainV3 { float x, y, z; } RainV3;
typedef struct RainVert { RainV3 pos; RainV3 dir; unsigned int cnt; unsigned char c[4]; } RainVert;
typedef struct RainRwGlobals { char pad[0x108]; void *(*rwmalloc)(unsigned int size, unsigned int hint); } RainRwGlobals;
MGLOBAL(RainRwGlobals *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(RainVert *, DAT_00771530, 0x00771530);
MGLOBAL(RainV3 *, DAT_0077152c, 0x0077152c);
MGLOBAL(float, DAT_006146c0, 0x006146c0);
MGLOBAL(float, DAT_006146c4, 0x006146c4);
MGLOBAL(float, DAT_00771538, 0x00771538);
MGLOBAL(float, DAT_0077153c, 0x0077153c);
MGLOBAL(float, DAT_006146c8, 0x006146c8);
MGLOBAL(float, DAT_006146cc, 0x006146cc);
MGLOBAL(float, DAT_006146d0, 0x006146d0);
MGLOBAL(float, DAT_006146d4, 0x006146d4);
MGLOBAL(float, DAT_006146d8, 0x006146d8);
MGLOBAL(float, DAT_006146dc, 0x006146dc);
MGLOBAL(float, DAT_006146e0, 0x006146e0);
MGLOBAL(float, DAT_006146e4, 0x006146e4);
MGLOBAL(float, DAT_006146b0, 0x006146b0);
MGLOBALA(float, DAT_00869cc0, 0x1f0, 0x00869cc0);
MGLOBAL(int, DAT_0086a480, 0x0086a480);
MGLOBAL(int, DAT_0086a484, 0x0086a484);
MGLOBAL(int, DAT_0086a488, 0x0086a488);
MGLOBAL(int, DAT_0086a48c, 0x0086a48c);
MGLOBAL(int, DAT_0086a490, 0x0086a490);
MGLOBAL(int, DAT_0086a494, 0x0086a494);
float FUN_00472650(float lo, float hi);
float FUN_004c39b0(RainV3 *out, RainV3 *in);
void FUN_00491070(void);
// MATCH: 0x00491590 FUN_00491590
int FUN_00491590(void)
{
  int i;
  RainVert *v;
  RainV3 *p;
  float r, a;

  DAT_00771530 = (RainVert *)DAT_007d3ff8->rwmalloc(0x7000, 0);
  DAT_0077152c = (RainV3 *)DAT_007d3ff8->rwmalloc(0x2a00, 0);
  v = DAT_00771530;
  if (v == 0) {
    return 0;
  }
  for (i = 0; i < 0x380; i++) {
    v[i].pos.x = FUN_00472650(DAT_006146c0, DAT_006146c4);
    v[i].pos.y = FUN_00472650(DAT_00771538, DAT_0077153c);
    v[i].pos.z = FUN_00472650(DAT_006146c8, DAT_006146cc);
    v[i].dir.x = FUN_00472650(DAT_006146d0, DAT_006146d4);
    v[i].dir.y = FUN_00472650(DAT_006146d8, DAT_006146dc);
    v[i].dir.z = FUN_00472650(DAT_006146e0, DAT_006146e4);
    FUN_004c39b0(&v[i].dir, &v[i].dir);
    v[i].dir.x = DAT_006146b0 * v[i].dir.x;
    v[i].dir.y = DAT_006146b0 * v[i].dir.y;
    v[i].cnt = i & 0x1f;
    v[i].dir.z = DAT_006146b0 * v[i].dir.z;
    r = FUN_00472650(0.0f, 14.0f);
    DAT_0077152c[i].x = r;
    p = &DAT_0077152c[i];
    a = r * 0.45f;
    p->y = FUN_00472650(-a, a);
    a = p->x * 0.6f;
    DAT_0077152c[i].z = FUN_00472650(-a, a);
  }
  for (i = 0; i < 0x1f0; i++) {
    DAT_00869cc0[i] = FUN_00472650(-8.0f, 8.0f);
  }
  DAT_0086a480 = 0;
  DAT_0086a48c = 1;
  DAT_0086a484 = 0;
  DAT_0086a490 = 2;
  DAT_0086a488 = 0;
  DAT_0086a494 = 3;
  FUN_00491070();
  return 1;
}
