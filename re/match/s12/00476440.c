// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x00476440 (h2): 100%, raw 37.3%. Typed records: 16 x 0xd8 at 0x00691698 (+0xc0 dur, +0xc4 t,
   +0xc8 ratio, +0xcc int active), 0x40-byte records at 0x00691618/0x00692418/0x00692498. First loop is an index
   loop (the compiler strength-reduces it to &rec[i].t vs 0x6924dc); the other two are (int) pointer loops.
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
typedef struct { unsigned char b[0x40]; } Rec40;
typedef struct {
  unsigned char pad[0xc0];
  float dur;     /* +0xc0 */
  float t;       /* +0xc4 */
  float ratio;   /* +0xc8 */
  int active;    /* +0xcc */
  unsigned char pad2[0x08];
} RecD8;
MGLOBAL(float, DAT_007f100c, 0x007f100c);
MGLOBALA(Rec40, DAT_00691618, 2, 0x00691618);
MGLOBALA(RecD8, DAT_00691698, 16, 0x00691698);
MGLOBALA(Rec40, DAT_00692418, 2, 0x00692418);
MGLOBAL(Rec40, DAT_00692498, 0x00692498);
void FUN_00475f90(RecD8 *p0);
void FUN_00476df0(Rec40 *p0);
// MATCH: 0x00476440 FUN_00476440
void FUN_00476440(void)
{
  int i;
  RecD8 *r;
  Rec40 *p;

  for (i = 0; i < 16; i++) {
    r = &DAT_00691698[i];
    if (r->active) {
      if (r->t > r->dur) {
        r->active = 0;
      } else {
        FUN_00475f90(r);
        r->t = DAT_007f100c + r->t;
        r->ratio = r->t / r->dur;
      }
    }
  }
  for (p = DAT_00691618; (int)p < (int)(DAT_00691618 + 2); p++) {
    FUN_00476df0(p);
  }
  for (p = DAT_00692418; (int)p < (int)(DAT_00692418 + 2); p++) {
    FUN_00476df0(p);
  }
  FUN_00476df0(&DAT_00692498);
}
