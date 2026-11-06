// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x0046d880 (helper h4): 100% by whole-symbol bytes (match.py --bytes: 511 B BYTE-IDENTICAL, relocations masked); the scored 98.7% is only the add esp/ret past Ghidra's 508 B size. Raw 54.1%. Moves: scalar float locals for the triangle's a/b/c/n (a kept on the x87 stack, b/c/n copied via int regs), a separate walking pointer t so param_2's home slot is reused for px, nested positive ifs with >= (continue + < gave test ah,5), and !(e < -0.25f) for the lower band. Literals -0.25f/0.05f/0.3f (.rdata 0x005cea5c/0x005cc9a0/0x005cc99c). Wheel +0x30 hit flag is an int (written 1). */
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
typedef struct { float x, y, z; } V3_0046d880;
typedef struct { char p0[0x30]; int hit; char p1[0xc4 - 0x34]; } Wheel_0046d880;
typedef struct {
    char p0[0x16c];
    Wheel_0046d880 w[4];              /* +0x16c, stride 0xc4 */
    char p1[0x9c8 - 0x47c];
    V3_0046d880 nrm;                  /* +0x9c8 */
    char p5[0xd04 - 0x9d4];
} Car_0046d880;
typedef struct {
    V3_0046d880 a, b, c, n;           /* +0x00 */
    char p0[0x6c - 0x30];
    V3_0046d880 e0, e1, e2;           /* +0x6c */
} Tri_0046d880;
MGLOBAL(unsigned int, DAT_0088e60c, 0x0088e60c);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBALA(int, DAT_0088e5e0, 4, 0x0088e5e0);
MGLOBALA(V3_0046d880, DAT_0088e620, 4, 0x0088e620);
// MATCH: 0x0046d880 FUN_0046d880
int FUN_0046d880(Car_0046d880 *param_1, Tri_0046d880 *param_2)
{
  int cnt;
  unsigned int k;
  int j;
  Tri_0046d880 *t;
  float ax, ay, az, bx, by, bz, cx, cy, cz, nx, ny, nz, px, py, pz;
  float dx, dy, dz, e;
  cnt = 0;
  t = param_2;
  for (k = 0; k < DAT_0088e60c; k++, t++) {
    ax = t->a.x;
    ay = t->a.y;
    az = t->a.z;
    bx = t->b.x;
    by = t->b.y;
    bz = t->b.z;
    cx = t->c.x;
    cy = t->c.y;
    cz = t->c.z;
    nx = t->n.x;
    ny = t->n.y;
    nz = t->n.z;
    for (j = 0; j < 4; j++) {
      if (DAT_0088e5e0[j] == 0) {
        px = DAT_0088e620[j].x;
        py = DAT_0088e620[j].y;
        pz = DAT_0088e620[j].z;
        if ((px - ax) * t->e0.x + (py - ay) * t->e0.y + (pz - az) * t->e0.z >= DAT_005d757c &&
            (px - bx) * t->e1.x + (py - by) * t->e1.y + (pz - bz) * t->e1.z >= DAT_005d757c) {
          dx = px - cx;
          dy = py - cy;
          dz = pz - cz;
          if (dx * t->e2.x + dy * t->e2.y + dz * t->e2.z >= DAT_005d757c) {
            e = dx * nx + dy * ny + dz * nz;
            if (!(e < -0.25f) && e < 0.05f &&
                nx * param_1->nrm.x + ny * param_1->nrm.y + nz * param_1->nrm.z > 0.3f) {
              DAT_0088e5e0[j] = 1;
              cnt++;
              param_1->w[j].hit = 1;
            }
          }
        }
      }
    }
  }
  return cnt;
}
