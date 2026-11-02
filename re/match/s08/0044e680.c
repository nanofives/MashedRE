// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x0044e680: 100% by hand (raw 21.0%). The 80 records are 0xf8 bytes from 0x00890060 (the
   0x00890080 record base used by the sibling files is this +0x20). What moved the score: the 80-float table as
   one initialized local float[8][10] (the compiler emits the 80 movs), kind/idx/rank/slot read into locals at
   the top (slot lives in edi), the table value in a local before the kind == 1 split (fadd/fsub st(1)),
   DAT_00894e00[kind][idx] indexed directly (the row local only for the table: indexing it by row moved the loop
   base pointer from +0x30 to +0x34, 93.2%), the identity matrix as the chained RwMatrixSetIdentity form.
   0x005ce2e0 / 0x005ce2e4 are the .rdata literals 146.0f / -122.0f. */
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
typedef struct { float x, y, z; } V3_0044e680;
typedef struct { float x, y, z, r; } Sphere_0044e680;
typedef struct {
  V3_0044e680 right; unsigned int flags;
  V3_0044e680 up; unsigned int pad1;
  V3_0044e680 at; unsigned int pad2;
  V3_0044e680 pos; unsigned int pad3;
} Matrix_0044e680;
typedef struct {
  Sphere_0044e680 sphere; /* 0x00 */
  V3_0044e680 pos;        /* 0x10 */
  int index;              /* 0x1c */
  char *obj;              /* 0x20 */
  char pad24[4];
  int rank;               /* 0x28 */
  int kind;               /* 0x2c */
  int idx;                /* 0x30 */
  int slot;               /* 0x34 */
  char pad38[0x10];
  float f48;              /* 0x48 */
  char pad4c[0x20];
  int i6c;                /* 0x6c */
  char pad70[0x88];
} Rec_0044e680;
MGLOBALA(Rec_0044e680, DAT_00890060, 80, 0x00890060);
typedef float Row_0044e680[4];
MGLOBALA(Row_0044e680, DAT_00894e00, 2, 0x00894e00);
MGLOBALA(V3_0044e680, DAT_00894da0, 3, 0x00894da0);
void FUN_004c1480(int p0, Matrix_0044e680 *p1, int p2);
void FUN_004c51a0(Matrix_0044e680 *p0, V3_0044e680 *p1, int p2);
void FUN_004e5fc0(char *p0);
// MATCH: 0x0044e680 FUN_0044e680
void FUN_0044e680(void)
{
  V3_0044e680 v;
  float tbl[8][10] = {
    { 10.0f, 35.0f, 60.0f, 85.0f, 110.0f, 135.0f, 160.0f, 185.0f, 210.0f, 235.0f },
    { 15.0f, 25.0f, 65.0f, 75.0f, 115.0f, 140.0f, 155.0f, 175.0f, 200.0f, 230.0f },
    { 10.0f, 30.0f, 60.0f, 90.0f, 115.0f, 130.0f, 150.0f, 180.0f, 215.0f, 240.0f },
    { 15.0f, 30.0f, 50.0f, 80.0f, 100.0f, 135.0f, 165.0f, 190.0f, 210.0f, 225.0f },
    { 15.0f, 40.0f, 65.0f, 85.0f, 100.0f, 125.0f, 160.0f, 195.0f, 210.0f, 235.0f },
    { 10.0f, 35.0f, 60.0f, 80.0f, 110.0f, 135.0f, 155.0f, 185.0f, 205.0f, 230.0f },
    { 15.0f, 30.0f, 55.0f, 95.0f, 115.0f, 130.0f, 165.0f, 180.0f, 215.0f, 240.0f },
    { 10.0f, 40.0f, 60.0f, 85.0f, 120.0f, 140.0f, 165.0f, 185.0f, 210.0f, 230.0f }
  };
  Matrix_0044e680 m;
  int i;
  float base;
  float t;
  int kind;
  int row;
  int idx;
  int rank;
  int slot;
  Rec_0044e680 *r;

  for (i = 0; i < 80; i++) {
    r = &DAT_00890060[i];
    kind = r->kind;
    idx = r->idx;
    rank = r->rank;
    slot = r->slot;
    row = idx + kind * 4;
    v.x = DAT_00894e00[kind][idx];
    r->index = i;
    v.y = DAT_00894da0[rank].y;
    if (kind == 1) {
      base = -122.0f;
    }
    else {
      base = 146.0f;
    }
    t = tbl[row][slot];
    if (kind == 1) {
      v.z = t + base;
    }
    else {
      v.z = base - t;
    }
    r->pos = v;
    m.right.x = m.up.y = m.at.z = 1.0f;
    m.right.y = m.right.z = m.up.x = 0.0f;
    m.up.z = m.at.x = m.at.y = 0.0f;
    m.pos.x = m.pos.y = m.pos.z = 0.0f;
    m.flags = m.flags | 0x20003;
    FUN_004c51a0(&m, &v, 0);
    FUN_004c1480(*(int *)(r->obj + 4), &m, 0);
    if ((*(unsigned char *)(r->obj + 0x4c) & 2) != 0) {
      FUN_004e5fc0(r->obj);
    }
    r->sphere = *(Sphere_0044e680 *)(r->obj + 0x1c);
    r->f48 = 1.0f;
    r->i6c = 1;
  }
}
