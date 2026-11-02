// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x0044be70 (289 B): 89.7% (regs 89.7%, ops 94.9%), by hand. Raw converter 40.5%.
   Sibling of 0x0044bd50 (same source shape, see that file): points from param+0xe4 / matrix param+0xa4,
   color 0x60ffffff, then verts[0] and verts[1] colors set to 0x00ffffff, raster from **(param+0x114).
   Same residual as 0x0044bd50: (b) points induction base pts+8 (z) vs the original pts+4 (y) and the loop
   store order. Permuter 300 s: no gain. relocheck OK, cxx ok. */
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
typedef struct V3_be70 {
  float x;
  float y;
  float z;
} V3_be70;
typedef struct Im3DVertex_be70 {
  V3_be70 m_pos;
  V3_be70 m_normal;
  uint m_color;
  float m_u;
  float m_v;
} Im3DVertex_be70;
typedef struct RwGlobals_be70 {
  void *m_00[8];
  int (*m_20)(int state, int value);
} RwGlobals_be70;
MGLOBAL(RwGlobals_be70 *, DAT_007d3ff8, 0x007d3ff8);
int FUN_004c3d90(V3_be70 *p0, int p1, int p2, int p3);
int FUN_004cd070(Im3DVertex_be70 *p0, int p1, int p2, int p3);
int FUN_004cd140(void);
int FUN_004cd2d0(int p0);
static __inline void SetUV_be70(Im3DVertex_be70 *vert, float u, float v)
{
  vert->m_u = u;
  vert->m_v = v;
}
static __inline void SetPos_be70(Im3DVertex_be70 *vert, V3_be70 *p)
{
  vert->m_pos.x = p->x;
  vert->m_pos.y = p->y;
  vert->m_pos.z = p->z;
}
// MATCH: 0x0044be70 FUN_0044be70
void FUN_0044be70(int param_1)
{
  int i;
  Im3DVertex_be70 *vp;
  float v[4];
  float u[4];
  V3_be70 pts[4];
  Im3DVertex_be70 verts[4];

  u[0] = 0.0f;
  u[1] = 1.0f;
  u[2] = 0.0f;
  u[3] = 1.0f;
  v[0] = 1.0f;
  v[1] = 1.0f;
  v[2] = 0.0f;
  v[3] = 0.0f;
  FUN_004c3d90(pts, param_1 + 0xe4, 4, param_1 + 0xa4);
  for (i = 0; i < 4; i++) {
    SetPos_be70(&verts[i], &pts[i]);
    SetUV_be70(&verts[i], u[i], v[i]);
    verts[i].m_color = 0x60ffffff;
  }
  verts[0].m_color = 0xffffff;
  verts[1].m_color = 0xffffff;
  if (*(int **)(param_1 + 0x114) != 0) {
    DAT_007d3ff8->m_20(1, **(int **)(param_1 + 0x114));
  } else {
    DAT_007d3ff8->m_20(1, 0);
  }
  DAT_007d3ff8->m_20(0xc, 1);
  if (FUN_004cd070(verts, 4, 0, 1)) {
    FUN_004cd2d0(4);
    FUN_004cd140();
  }
}
