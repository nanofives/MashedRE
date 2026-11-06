// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x0046cc40 (1687 B): 73.1% (regs 76.1, ops 92.5), by hand (h3). Raw 22.7%.
   What moved it: the debug ring at 0x007e9de0 as ONE struct (count n, then type/frame/id[256], 32-byte
   records[256], 16-dword payloads[256*16]) so every store re-reads n and the payload copy stays an element
   loop (separate arrays gave rep movsd and a cached n: 23% -> 72%), pointer induction over the four
   contact points (float * at .y, compare against &DAT_0088e620[4].y), wheel records and flags in the
   order flag / point / wheel, and !(dist < -0.25f) for the lower band. The DBG_PUSH macro is the
   three inlined copies; ev[16] is an uninitialised local in the original too (its slots are only read).
   Residual (b): frame slot assignment of x/y/z/dist and the induction pointers, prologue push order (the
   original pushes esi and loads t before the n == 0 guard), the original hoists ev[6] into ebp where ours
   hoists ev[7], and block layout (the original places the loop tail right after the 0xff010101 branch and
   the FUN_00468b40 branch after the epilogue; ours the reverse). Tried: do/while vs for, if/do-while guard,
   branch swap (64%), the permuter 300 s (no improvement). */
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
typedef struct { float x, y, z; } V3;
typedef struct {
    V3 p0;                          /* 0x00 */
    V3 p1;                          /* 0x0c */
    V3 p2;                          /* 0x18 */
    V3 n;                           /* 0x24 */
    int f30;                        /* 0x30 */
    int key;                        /* 0x34 */
    char p38[0x6c - 0x38];
    V3 n0;                          /* 0x6c */
    V3 n1;                          /* 0x78 */
    V3 n2;                          /* 0x84 */
} Tri;                              /* 0x90 */
typedef struct {
    float dist;                     /* 0x00 */
    char p04[0x58 - 4];
    int f58;                        /* 0x58 */
    int f5c;                        /* 0x5c */
    char p60[0x6c - 0x60];
    V3 n;                           /* 0x6c */
    char p78[0xc4 - 0x78];
} Wheel;                            /* 0xc4 */
typedef struct {
    int id;                         /* 0x000 */
    char p004[0x194 - 4];
    Wheel w[4];                     /* 0x194 */
    char p4a4[0x9c8 - 0x4a4];
    V3 v9c8;                        /* 0x9c8 */
    char p9d4[0x9f0 - 0x9d4];
    int f9f0;                       /* 0x9f0 */
    int f9f4;                       /* 0x9f4 */
} Car;
typedef struct { int c[8]; } DbgRec;   /* 0x20 */
MGLOBAL(uint, DAT_0088e60c, 0x0088e60c);
MGLOBALA(int, DAT_0088e5e0, 4, 0x0088e5e0);
MGLOBALA(V3, DAT_0088e620, 4, 0x0088e620);
MGLOBAL(int, DAT_0088e650, 0x0088e650);
MGLOBAL(int, DAT_007f1030, 0x007f1030);
typedef struct {
    uint n;                         /* 0x0000 */
    int type[256];                  /* 0x0004 */
    int frame[256];                 /* 0x0404 */
    int id[256];                    /* 0x0804 */
    DbgRec rec[256];                /* 0x0c04 */
    int m[256 * 16];                /* 0x2c04 */
} DbgBuf;
MGLOBAL(DbgBuf, DAT_007e9de0, 0x007e9de0);
int FUN_0040e350(void);
int FUN_00468b40(Tri *t, Car *c);

#define DBG_PUSH(col) \
    DAT_007e9de0.type[DAT_007e9de0.n] = 6; \
    DAT_007e9de0.frame[DAT_007e9de0.n] = DAT_007f1030; \
    DAT_007e9de0.id[DAT_007e9de0.n] = c->id + 0xff00; \
    DAT_007e9de0.rec[DAT_007e9de0.n].c[0] = (col); \
    DAT_007e9de0.rec[DAT_007e9de0.n].c[1] = ev[1]; \
    DAT_007e9de0.rec[DAT_007e9de0.n].c[2] = ev[2]; \
    DAT_007e9de0.rec[DAT_007e9de0.n].c[3] = ev[3]; \
    DAT_007e9de0.rec[DAT_007e9de0.n].c[4] = ev[4]; \
    DAT_007e9de0.rec[DAT_007e9de0.n].c[5] = ev[5]; \
    DAT_007e9de0.rec[DAT_007e9de0.n].c[6] = ev[6]; \
    DAT_007e9de0.rec[DAT_007e9de0.n].c[7] = ev[7]; \
    for (k = 0; k < 16; k++) { \
      DAT_007e9de0.m[DAT_007e9de0.n * 16 + k] = ev[k]; \
    } \
    DAT_007e9de0.n++; \
    if (DAT_007e9de0.n >= 0x100) { \
      DAT_007e9de0.n = 0; \
    }

// MATCH: 0x0046cc40 FUN_0046cc40
void FUN_0046cc40(Car *c, Tri *t)
{
  uint i;
  int k, m, key;
  int *flag;
  float *pt;
  Wheel *w;
  float p0x, p0y, p0z, p1x, p1y, p1z, p2x, p2y, p2z, nx, ny, nz;
  float x, y, z, dx, dy, dz, dist;
  int ev[16];

  for (i = 0; i < DAT_0088e60c; i++, t++) {
    p0x = t->p0.x; p0y = t->p0.y; p0z = t->p0.z;
    p1x = t->p1.x; p1y = t->p1.y; p1z = t->p1.z;
    p2x = t->p2.x; p2y = t->p2.y; p2z = t->p2.z;
    nx = t->n.x; ny = t->n.y; nz = t->n.z;
    flag = DAT_0088e5e0;
    pt = &DAT_0088e620[0].y;
    w = c->w;
    do {
      if (*flag == 0) {
        x = pt[-1];
        y = pt[0];
        z = pt[1];
        if ((z - p0z) * t->n0.z + (y - p0y) * t->n0.y + (x - p0x) * t->n0.x >= 0.0f &&
            (z - p1z) * t->n1.z + (y - p1y) * t->n1.y + (x - p1x) * t->n1.x >= 0.0f) {
          dx = x - p2x;
          dy = y - p2y;
          dz = z - p2z;
          if (dz * t->n2.z + dy * t->n2.y + dx * t->n2.x >= 0.0f) {
            dist = dz * nz + dy * ny + dx * nx;
            if (!(dist < -0.25f) && dist < 0.25f &&
                nz * c->v9c8.z + ny * c->v9c8.y + nx * c->v9c8.x > 0.3f) {
              key = t->key;
              if (key == (int)0xff010101) {
                if (c->f9f0 == 0) {
                  m = FUN_0040e350();
                  if (m != 1 && m != 2 && m != 3 && m != 4 && m != 5) {
                    DBG_PUSH(key)
                  }
                }
              } else if (FUN_00468b40(t, c) == 0) {
                w->n = t->n;
                w->dist = dist;
                w->f58 = t->f30;
                w->f5c = t->key;
                if (key != (int)0xff0000ff) {
                  if (c->f9f0 == 1 && key != (int)0xffff0080 && c->f9f4 > 0x32) {
                    m = FUN_0040e350();
                    if (m != 1 && m != 2 && m != 3 && m != 4 && m != 5) {
                      DBG_PUSH(0xff0000ff)
                    }
                  }
                } else {
                  if (c->f9f0 == 0 || c->f9f0 == 1) {
                    m = FUN_0040e350();
                    if (m != 1 && m != 2 && m != 3 && m != 4 && m != 5) {
                      DBG_PUSH(0xff0000ff)
                    }
                  }
                }
                *flag = 1;
                DAT_0088e650++;
                if (DAT_0088e650 == 4) {
                  return;
                }
              }
            }
          }
        }
      }
      pt += 3;
      w++;
      flag++;
    } while (pt < &DAT_0088e620[4].y);
  }
}
