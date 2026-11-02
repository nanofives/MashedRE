// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 h6 0x00491c00: 100% (regs 100%). FILE-STATIC globals: DAT_00771724 and DAT_00771728 are static under C (pilot finding 2: the original keeps both in registers across stores through pointers and reloads DAT_007dc57c), MGLOBAL under C++. Hand: V2/Quad/Sph struct copies for the 0.1/0.1, 0,0,1,1 and 0,0,0,1000 locals (scalars were constant-folded), RwMalloc through the RwGlobals slot +0x108, Spark array indexing for the zero loop, tail call to FUN_00491bd0. Permuter: DAT_00771724 = a moved above the plugin stores (fixes the [ecx+eax] operand order). Residual: none. */
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
typedef struct {
  int active;
  int age;
  float pos[3];
  float vel[3];
} Spark;
typedef struct {
  unsigned char *p;
  int stride;
} LockCol;
typedef struct {
  float x;
  float y;
  float z;
  float r;
} Sph;
typedef struct {
  float a;
  float b;
  float c;
  float d;
} Quad;
typedef struct {
  float x;
  float y;
} V2;
typedef void *(*RwMallocFn)(unsigned int size, unsigned int hint);
MGLOBAL(Spark *, DAT_00869ca0, 0x00869ca0);
MGLOBAL(int, DAT_00771718, 0x00771718);
#ifndef __cplusplus
static int DAT_00771724;
#else
MGLOBAL(int, DAT_00771724, 0x00771724);
#endif
#ifndef __cplusplus
static int DAT_00771728;
#else
MGLOBAL(int, DAT_00771728, 0x00771728);
#endif
MGLOBAL(int, DAT_007dc57c, 0x007dc57c);
MGLOBAL(char *, DAT_007d3ff8, 0x007d3ff8);
int FUN_004c0b30(void);
int FUN_00534b60(int p0, int p1, int p2);
int FUN_004e7e30(int p0, int p1);
int FUN_004e8090(int p0, int p1);
int FUN_00535700(int p0, void *p1, int p2, int p3);
void FUN_00491bd0(void);
// MATCH: 0x00491c00 FUN_00491c00
void FUN_00491c00(int param_1)
{
  int frame;
  int i;
  int a;
  LockCol col;
  V2 sz;
  Sph sph;
  Quad q;

  frame = FUN_004c0b30();
  q.a = 0.0f;
  q.b = 0.0f;
  q.c = 1.0f;
  q.d = 1.0f;
  sz.x = 0.1f;
  sz.y = 0.1f;
  DAT_00869ca0 = (Spark *)(*(RwMallocFn *)(DAT_007d3ff8 + 0x108))(0x4000, 0);
  sph.x = 0.0f;
  sph.y = 0.0f;
  sph.z = 0.0f;
  sph.r = 1000.0f;
  DAT_00771728 = param_1;
  a = FUN_00534b60(0x200, 0x80007, 0);
  DAT_00771724 = a;
  *(int *)(*(int *)(DAT_007dc57c + a) + 0xa4) = 5;
  *(int *)(*(int *)(DAT_007dc57c + a) + 0xa8) = 2;
  *(unsigned int *)(*(int *)(DAT_007dc57c + a) + 0x40) |= 0x10000000;
  *(unsigned int *)(*(int *)(DAT_007dc57c + a) + 0x40) |= 0x800000;
  *(int *)(*(int *)(DAT_007dc57c + a) + 4) = 0x200;
  FUN_004e7e30(a, frame);
  *(Sph *)(*(int *)(*(int *)(DAT_00771724 + 0x18) + 0x5c) + 4) = sph;
  *(int *)(*(int *)(DAT_00771724 + DAT_007dc57c) + 0xac) = 1;
  *(Quad *)(*(int *)(DAT_00771724 + DAT_007dc57c) + 0xd8) = q;
  *(unsigned int *)(*(int *)(DAT_00771724 + DAT_007dc57c) + 0x40) |= 0x80000;
  *(V2 *)(*(int *)(DAT_00771724 + DAT_007dc57c) + 0xb8) = sz;
  *(unsigned int *)(*(int *)(DAT_00771724 + DAT_007dc57c) + 0x40) |= 0x4000;
  if (DAT_00771728 != 0) {
    *(unsigned char *)(DAT_00771728 + 0x50) = 2;
    FUN_004e8090(**(int **)(*(int *)(DAT_00771724 + 0x18) + 0x20), DAT_00771728);
  }
  FUN_00535700(DAT_00771724, &col, 2, 0x40000000);
  for (i = 0x200; i != 0; i--) {
    col.p[0] = 0xff;
    col.p[1] = 0xff;
    col.p[2] = 0xff;
    col.p[3] = 0xff;
    col.p += col.stride;
  }
  for (i = 0; i < 0x200; i++) {
    DAT_00869ca0[i].active = 0;
    DAT_00869ca0[i].age = 0;
  }
  DAT_00771718 = 0;
  FUN_00491bd0();
}
