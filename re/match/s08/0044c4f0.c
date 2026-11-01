// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x0044c4f0 (250 B): 98.8% (regs 98.8%, ops 98.8%). Start: re/match/pilot/a2_0044c4f0.c (98.8%),
   moved to the s08 form (globals through MGLOBAL). Pilot findings kept: FUN_004b3f90 takes 1 argument and the
   other six pushes belong to FUN_0047fad0; 0x3f7ff972 = 0.9999f; -t*k through a negated temp; 0.5f (0x005cc32c)
   and 0.875f (0x005cd0b4) as literals; 0x005cc320 (1.0f) stays a global (literal 1.0f gives 93.8%).
   Residual (1 instruction): mov [ebx+0x3c],eax is scheduled after the two pushes for FUN_0047cdc0 instead of
   before. Tried here on top of the pilot's attempts: void-pointer handle type, s->h assigned from the call and
   re-read as the argument, int-typed second argument, int return on FUN_0047cdc0: no move. Class (a) candidate:
   store-vs-push scheduling. relocheck OK, cxx ok. */
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
typedef struct Tmpl6_c4f0 {
  float f[6];
} Tmpl6_c4f0;
typedef struct Slot_c4f0 {
  int a, b, c, d;          /* +0x00 */
  float k[4];              /* +0x10 */
  Tmpl6_c4f0 t;            /* +0x20 */
  int n;                   /* +0x38 */
  int h;                   /* +0x3c */
  float v[3];              /* +0x40 */
  int z0;                  /* +0x4c */
  float w;                 /* +0x50 */
  int z1;                  /* +0x54 */
} Slot_c4f0;
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(Tmpl6_c4f0, DAT_005f9730, 0x005f9730);
float FUN_004728f0(int a);
void FUN_0047cdc0(int h, float f);
int FUN_0047fad0(int a, float *v, int z, float f0, float f1, float f2, int e);
int FUN_004b3f90(int a);
// MATCH: 0x0044c4f0 FUN_0044c4f0
void FUN_0044c4f0(Slot_c4f0 *s, int a, int b, int c, int e)
{
  float k;
  float x;
  int h;

  s->a = a;
  s->b = b;
  s->c = c;
  s->d = 0;
  k = DAT_005cc320 / FUN_004728f0(a);
  s->t = DAT_005f9730;
  s->n = 9;
  s->z0 = 0;
  s->z1 = 0;
  s->k[0] = k * s->t.f[2];
  { float nt = -s->t.f[5]; s->k[1] = nt * k; }
  s->k[2] = k * s->t.f[0];
  { float nt3 = -s->t.f[3]; s->k[3] = nt3 * k; }
  x = (s->t.f[1] - s->t.f[4]) * 0.5f;
  s->w = x;
  s->v[0] = 0.875f * ((s->t.f[0] - s->t.f[3]) * 0.5f);
  s->v[1] = x * 0.875f;
  s->v[2] = 0.875f * ((s->t.f[2] - s->t.f[5]) * 0.5f);
  h = FUN_0047fad0(FUN_004b3f90(b), s->v, 0, 0.1f, 0.9999f, 80.0f, e + 0x75ab);
  s->h = h;
  FUN_0047cdc0(h, 100.0f);
}
