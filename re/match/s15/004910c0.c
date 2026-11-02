// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x004910c0 (h5, by hand): 97.2% (regs 97.2%), 626 B whole symbol, 5 differing bytes. Key moves: viewWindow copied as one RwV2d-shaped struct (81% -> 96.7%), the far plane read as (float)(farPlane + 0.0) to get the original fld/fstp copy (a plain float copy emits mov). Permuter (301 compiles, no gain). Residual (b): that fstp is scheduled after the next push ebx in ours, before it in the original. FUN_004c1b10(cam, far) and FUN_004c1b40(cam, sphere) take the camera from a nested FUN_004671a0(0) call (Ghidra merged the argument). */
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
typedef struct RainSphere { RainV3 center; float radius; } RainSphere;
typedef struct RainV2 { float x, y; } RainV2;
typedef struct RainCam { char pad[0x68]; RainV2 vw; char pad2[0x84 - 0x70]; float farPlane; } RainCam;
MGLOBAL(int, DAT_00771534, 0x00771534);
MGLOBAL(RainVert *, DAT_00771530, 0x00771530);
MGLOBAL(float, DAT_006146b4, 0x006146b4);
MGLOBAL(int, DAT_0086a480, 0x0086a480);
MGLOBAL(int, DAT_0086a484, 0x0086a484);
MGLOBAL(int, DAT_0086a488, 0x0086a488);
MGLOBAL(int, DAT_0086a48c, 0x0086a48c);
MGLOBAL(int, DAT_0086a490, 0x0086a490);
MGLOBAL(int, DAT_0086a494, 0x0086a494);
RainCam *FUN_004671a0(int a);
float *FUN_00467210(int a);
float FUN_00472650(float lo, float hi);
RainCam *FUN_004c1b10(RainCam *cam, float farPlane);
int FUN_004c1b40(RainCam *cam, RainSphere *sphere);
float FUN_004c39b0(RainV3 *out, RainV3 *in);
// MATCH: 0x004910c0 FUN_004910c0
void FUN_004910c0(void)
{
  float far0;
  float *m;
  RainCam *cam;
  RainV2 vw;
  RainSphere sphere;
  RainV3 d;
  RainVert *v;
  int i, j;
  float px, py, pz, a, b;

  far0 = (float)(FUN_004671a0(0)->farPlane + 0.0);
  m = FUN_00467210(0);
  cam = FUN_004671a0(0);
  vw = cam->vw;
  sphere.radius = 0.1f;
  if (DAT_00771534 != 0) {
    FUN_004c1b10(FUN_004671a0(0), 0.8f);
    for (i = 0; i < 4; i++) {
      v = &DAT_00771530[i * 0xe0];
      for (j = 0; j < 0xe0; j++) {
        FUN_004c39b0(&d, &v->dir);
        d.x = DAT_006146b4 * d.x;
        d.y = DAT_006146b4 * d.y;
        d.z = DAT_006146b4 * d.z;
        v->pos.x = v->pos.x - d.x;
        v->pos.y = v->pos.y - d.y;
        v->pos.z = v->pos.z - d.z;
        sphere.center = v->pos;
        v->cnt++;
        if (FUN_004c1b40(FUN_004671a0(0), &sphere) == 0) {
          v->cnt = 0;
          px = FUN_00472650(0.5f, 10.5f);
          a = vw.y * px;
          py = FUN_00472650(-a, a);
          b = vw.x * px;
          pz = FUN_00472650(-b, b);
          v->pos.x = py * m[4] + px * m[8] + pz * m[0] + m[12];
          v->pos.y = pz * m[1] + py * m[5] + px * m[9] + m[13];
          v->pos.z = pz * m[2] + py * m[6] + px * m[10] + m[14];
          DAT_0086a480 = DAT_0086a480 + DAT_0086a48c;
          DAT_0086a484 = DAT_0086a484 + DAT_0086a490;
          DAT_0086a488 = DAT_0086a488 + DAT_0086a494;
          if (DAT_0086a480 >= 0x1f0) {
            DAT_0086a480 = 0;
          }
          if (DAT_0086a484 >= 0x1f0) {
            DAT_0086a484 = 2;
          }
          if (DAT_0086a488 >= 0x1f0) {
            DAT_0086a488 = 1;
          }
        }
        v++;
      }
    }
    FUN_004c1b10(FUN_004671a0(0), far0);
  }
}
