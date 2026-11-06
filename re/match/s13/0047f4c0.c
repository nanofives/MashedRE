// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x0047f4c0 (h7): 100% (regs 100%). Raw 58.0%. 124-byte descriptor + 32-byte AABB (+-1000) on the
   stack; FUN_0055dec0 takes 1 arg and FUN_0055af40 takes (FUN_0055dec0(world), matrix); the nine bit sets are
   one macro `bits[(a*n + b) >> 5] |= 1 << ((a*n + b) & 31)` with an unsigned row width n (shr, not sar); the
   compiler picks the in-place `or` vs load/or/store per site. 200-int fill as a for loop (rep stosd). */
/* ---- MATCH-S13 preamble (same text in every re/match/s13 source; not part of any function body) ----
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
typedef struct { V3 sup; int pad0; V3 inf; int pad1; } BBox;
typedef struct { int pad0; unsigned int n; int pad8; unsigned int bits[1]; } BitMat;
typedef struct { int w[31]; } Desc7c;
MGLOBAL(BitMat *, DAT_0086ecc0, 0x0086ecc0);
MGLOBALA(int, DAT_006c6eb8, 200, 0x006c6eb8);
void FUN_0055af40(int p0, BitMat *p1);
int FUN_0055dc70(Desc7c *p0, float p1, BBox *p2);
int FUN_0055dec0(int p0);
BitMat *FUN_00562520(int p0, int p1);
void FUN_00562580(Desc7c *p0);
void FUN_005625e0(Desc7c *p0, int p1);
void FUN_0057c4b0(Desc7c *p0, int p1, int p2);
#define SETBIT(m, a, b) ((m)->bits[((a) * (m)->n + (b)) >> 5] |= 1 << (((a) * (m)->n + (b)) & 0x1f))
// MATCH: 0x0047f4c0 FUN_0047f4c0
int FUN_0047f4c0(float param_1)
{
  BBox box;
  Desc7c desc;
  int world;
  int i;

  box.pad0 = 0;
  box.pad1 = 0;
  box.sup.x = 1000.0f;
  box.sup.y = 1000.0f;
  box.sup.z = 1000.0f;
  box.inf.x = -1000.0f;
  box.inf.y = -1000.0f;
  box.inf.z = -1000.0f;
  FUN_00562580(&desc);
  FUN_0057c4b0(&desc, 200, 100);
  FUN_005625e0(&desc, 0x20);
  world = FUN_0055dc70(&desc, param_1, &box);
  DAT_0086ecc0 = FUN_00562520(10, 10);
  SETBIT(DAT_0086ecc0, 1, 1);
  SETBIT(DAT_0086ecc0, 2, 2);
  SETBIT(DAT_0086ecc0, 3, 3);
  SETBIT(DAT_0086ecc0, 4, 4);
  SETBIT(DAT_0086ecc0, 5, 5);
  SETBIT(DAT_0086ecc0, 6, 6);
  SETBIT(DAT_0086ecc0, 7, 7);
  SETBIT(DAT_0086ecc0, 7, 1);
  SETBIT(DAT_0086ecc0, 8, 8);
  FUN_0055af40(FUN_0055dec0(world), DAT_0086ecc0);
  for (i = 0; i < 200; i++) {
    DAT_006c6eb8[i] = 1;
  }
  return world;
}
