// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x00491340 (h5, by hand): 98.1% (regs 98.1%). Tried: counter ++ in 5 positions/forms (++v->cnt in the if is best), -= forms, separate scale locals, pointer to d, int vs unsigned counter, permuter (216 compiles, no gain). Table point loaded into px/py/pz locals (needed: 77.6% -> 95.3%). Residual (b): one fld [edi] scheduled after the counter load in ours, before it in the original. */
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
typedef struct RainVert { float x, y, z; float dx, dy, dz; unsigned int cnt; unsigned char c[4]; } RainVert;
typedef struct RainV3 { float x, y, z; } RainV3;
MGLOBAL(int, DAT_00771534, 0x00771534);
MGLOBAL(RainVert *, DAT_00771530, 0x00771530);
MGLOBAL(float, DAT_006146b4, 0x006146b4);
MGLOBAL(RainV3 *, DAT_0077152c, 0x0077152c);
int FUN_004671a0(int a);
float *FUN_00467210(int a);
float FUN_004c39b0(RainV3 *out, float *in);
// MATCH: 0x00491340 FUN_00491340
void FUN_00491340(void)
{
  float *m;
  RainVert *v;
  RainV3 *p;
  RainV3 d;
  float px, py, pz;
  int i, j;

  FUN_004671a0(0);
  m = FUN_00467210(0);
  FUN_004671a0(0);
  if (DAT_00771534 != 0) {
    for (i = 0; i < 4; i++) {
      v = &DAT_00771530[i * 0xe0];
      for (j = 0; j < 0xe0; j++) {
        FUN_004c39b0(&d, &v->dx);
        d.x = DAT_006146b4 * d.x;
        d.y = DAT_006146b4 * d.y;
        d.z = DAT_006146b4 * d.z;
        v->x = v->x - d.x;
        v->y = v->y - d.y;
        v->z = v->z - d.z;
        if (++v->cnt > 7) {
          p = &DAT_0077152c[i * 0xe0 + j];
          v->cnt = 0;
          px = p->x;
          py = p->y;
          pz = p->z;
          v->x = py * m[4] + px * m[8] + pz * m[0] + m[12];
          v->y = pz * m[1] + py * m[5] + px * m[9] + m[13];
          v->z = pz * m[2] + py * m[6] + px * m[10] + m[14];
        }
        v++;
      }
    }
  }
}
