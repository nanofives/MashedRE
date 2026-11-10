// FLAGS: /O2 /GS /Ob1
/* TL-MATCH-LUA 0x00478660 (by hand, 2026-11-10): 79.1% (regs 79.1%, ops 93.9%), 444/446 ins. Raw g2s 22.6%.
   Shape: per AI polygon i, RwRGBA key {i, i>>8, 0, 0xff} -> FUN_004b5030 two 0x30 triangles; quad = tri 0 verts
   plus the first tri 1 vertex not equal (FUN_004b46b0) to any of them; normal via the register-arg 0x004785e0
   (HELD static copy); centre via FUN_004b4550; local RwMatrix {right = (0,1,0) x at, up = at x right, at, pos},
   FUN_004c45f0 / FUN_004c4dc0 inverse / FUN_004c3d90 transform of the 4 corners; quadrant classify into c[0..3].
   Moves that scored: classify as a macro (an __inline helper stayed a call) 18.6 -> 41.0%; the commuted retests
   (`0.0f > x`) stop jump threading 41 -> 47%; p assigned just before the first use (ebp keeps &c[0], p = ebp-0x18)
   47 -> 79.1%. Residual (b): (1) the original re-tests x before the y test of c1 and c3 (no jump threading); the
   only spelling found that keeps that (`x < 0.0`) emits fcomp qword, the original fcomp dword [0x005d757c]; plain
   spellings thread (ours drops the retest, 404 ins), so this file keeps the commuted retest, which loads the
   constant first (fld K / fcomp x); (2) x87 scheduling of the right / up cross products; (3) lea edi from ebp
   instead of esi. Not SIB base/index order. Tried: declaration orders (no effect), pointer walk p++, a corner
   pointer, global float for 0.0, __inline Cross, local y-axis vector, separate tx/ty temps. */
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
typedef struct { float x, y, z; } RwV3d;
typedef struct { RwV3d right; unsigned int flags; RwV3d up; unsigned int pad1; RwV3d at; unsigned int pad2;
                 RwV3d pos; unsigned int pad3; } RwMatrix;
typedef struct { unsigned char red, green, blue, alpha; } RwRGBA;
typedef struct { RwV3d v[3]; RwV3d n; } PolyTri;      /* 0x30, FUN_004b5030 output */
typedef struct { RwV3d v[3]; } Tri3;
typedef struct { RwV3d at; RwV3d pos; RwV3d c[4]; int pad; } AiPoly; /* 0x4c, course block +0xc1c0 */

void FUN_004783f0(int p0);
void FUN_004987b0(const char *fmt, ...);
void FUN_004b4550(RwV3d *out, RwV3d *pts, int n);
int FUN_004b46b0(RwV3d *a, RwV3d *b);
int FUN_004b5030(int world, RwRGBA *key, PolyTri *out, int n);
float FUN_004c39b0(RwV3d *out, RwV3d *in);
RwV3d *FUN_004c3d90(RwV3d *out, RwV3d *in, int n, RwMatrix *m);
RwMatrix *FUN_004c45f0(RwMatrix *m);
RwMatrix *FUN_004c4dc0(RwMatrix *out, RwMatrix *in);

#ifndef MATCH_PORT
// HELD: 0x004785e0 (body in re/match/s12/004785e0.c; this copy exists only so the DDK compiles the caller as the original; not compiled in the port)
static void FUN_004785e0(float *p, float *out, int norm)
{
  float ax, ay, az;
  float b[3];

  ax = p[3] - p[0];
  ay = p[4] - p[1];
  az = p[5] - p[2];
  b[0] = p[6] - p[0];
  b[1] = p[7] - p[1];
  b[2] = p[8] - p[2];
  out[0] = b[2] * ay - b[1] * az;
  out[1] = b[0] * az - b[2] * ax;
  out[2] = b[1] * ax - b[0] * ay;
  if (norm) {
    FUN_004c39b0((RwV3d *)out, (RwV3d *)out);
  }
}
#else
void FUN_004785e0(float *p, float *out, int norm);
#endif

#define CORNER(p, t, q)   if ((t).x < 0.0f && (t).y < 0.0f) (p)->c[0] = (q);   else if (0.0f > (t).x && (t).y > 0.0f) (p)->c[1] = (q);   else if ((t).x > 0.0f && (t).y > 0.0f) (p)->c[2] = (q);   else if (0.0f < (t).x && (t).y < 0.0f) (p)->c[3] = (q)

// MATCH: 0x00478660 FUN_00478660
void FUN_00478660(int course, int world)
{
  int n;
  int i;
  int k;
  AiPoly *p;
  RwRGBA key;
  RwV3d q[4];
  RwV3d cand;
  RwV3d t[4];
  RwMatrix m;
  Tri3 t2;
  RwMatrix inv;
  PolyTri tri[2];

  n = *(int *)(world + 0x14);
  *(int *)(course + 0x16240) = n;
  FUN_004987b0("Number of AI polygons is %d\n", n);
  for (i = 0; i < n; i++) {
    key.red = (unsigned char)i;
    key.green = (unsigned char)(i >> 8);
    key.blue = 0;
    key.alpha = 0xff;
    FUN_004b5030(world, &key, tri, 2);
    *(Tri3 *)q = *(Tri3 *)tri[0].v;
    t2 = *(Tri3 *)tri[1].v;
    for (k = 0; k < 3; k++) {
      cand = t2.v[k];
      if (!FUN_004b46b0(&cand, &q[0]) && !FUN_004b46b0(&cand, &q[1]) && !FUN_004b46b0(&cand, &q[2])) {
        q[3] = cand;
        break;
      }
    }
    p = &((AiPoly *)(course + 0xc1c0))[i];
    FUN_004785e0(&q[0].x, &p->at.x, 1);
    FUN_004b4550(&p->pos, q, 4);
    m.pos = p->pos;
    m.at = p->at;
    m.right.x = m.at.z - m.at.y * 0.0f;
    m.right.y = m.at.x * 0.0f - m.at.z * 0.0f;
    m.right.z = m.at.y * 0.0f - m.at.x;
    FUN_004c39b0(&m.right, &m.right);
    m.up.x = m.right.z * m.at.y - m.right.y * m.at.z;
    m.up.y = m.right.x * m.at.z - m.right.z * m.at.x;
    m.up.z = m.right.y * m.at.x - m.right.x * m.at.y;
    FUN_004c39b0(&m.up, &m.up);
    FUN_004c45f0(&m);
    FUN_004c4dc0(&inv, &m);
    FUN_004c3d90(t, q, 4, &inv);
    CORNER(p, t[0], q[0]);
    CORNER(p, t[1], q[1]);
    CORNER(p, t[2], q[2]);
    CORNER(p, t[3], q[3]);
  }
  FUN_004783f0(course);
}
