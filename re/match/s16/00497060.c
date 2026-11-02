// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00497060 (h3): by hand, 100% (regs 100%), relocheck OK. View (0x00496ec0) and projection
   (0x00497000) matrices, an identity written in the D3DXMatrixIdentity chained-assignment shape, the
   multiply through the __stdcall thunk 0x004ed6ba, then four optional struct copies out. The first stack
   argument is not read. */
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
typedef struct Mat44 { float m[16]; } Mat44;
void FUN_00496ec0(Mat44 *out, void *camera);
void FUN_00497000(Mat44 *out, void *camera);
Mat44 *__stdcall FUN_004ed6ba(Mat44 *out, const Mat44 *a, const Mat44 *b);
// MATCH: 0x00497060 FUN_00497060
void FUN_00497060(void *param_1, void *camera, Mat44 *viewProj, Mat44 *proj, Mat44 *view, Mat44 *world)
{
  Mat44 mWorld;
  Mat44 mProj;
  Mat44 mView;
  Mat44 mViewProj;

  FUN_00496ec0(&mView, camera);
  FUN_00497000(&mProj, camera);
  mWorld.m[1] = mWorld.m[2] = mWorld.m[3] =
  mWorld.m[4] = mWorld.m[6] = mWorld.m[7] =
  mWorld.m[8] = mWorld.m[9] = mWorld.m[11] =
  mWorld.m[12] = mWorld.m[13] = mWorld.m[14] = 0.0f;
  mWorld.m[0] = mWorld.m[5] = mWorld.m[10] = mWorld.m[15] = 1.0f;
  FUN_004ed6ba(&mViewProj, &mProj, &mView);
  if (viewProj != 0) {
    *viewProj = mViewProj;
  }
  if (proj != 0) {
    *proj = mProj;
  }
  if (view != 0) {
    *view = mView;
  }
  if (world != 0) {
    *world = mWorld;
  }
}
