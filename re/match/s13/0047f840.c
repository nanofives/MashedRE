// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x0047f840 (h7): 100% (regs 100%). Raw 75.3%: the per-call add esp came from nested calls f(g(world), x) with the right arities (deb0/dec0 take 1 arg, fe10/ad20/fe30/fe40 take 2); gravity as a 3-float local; memset intrinsic for the 200-dword clear; callback as function pointer. relocheck OK. */
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
MGLOBAL(int, DAT_006ce274, 0x006ce274);
MGLOBAL(int, DAT_0086caa0, 0x0086caa0);
MGLOBALA(int, DAT_006c71d8, 200, 0x006c71d8);
void *memset(void *p0, int p1, unsigned int p2);
#pragma intrinsic(memset)
typedef struct { float x, y, z; } V3;
int FUN_0047f4c0(float p0);
int FUN_0047f7d0(int p0, int p1);
void FUN_0055ad20(int p0, float p1);
void FUN_0055bac0(int p0, int p1);
void FUN_0055c490(int p0, int (*p1)(int, int));
void FUN_0055c4a0(int p0, float p1);
void FUN_0055c4f0(int p0, float p1);
int FUN_0055ddd0(int p0, int p1, int p2);
int FUN_0055deb0(int p0);
int FUN_0055dec0(int p0);
void FUN_0055fe10(int p0, V3 *p1);
void FUN_0055fe30(int p0, int p1);
void FUN_0055fe40(int p0, int p1);
int FUN_00562600(int p0, int p1);
// MATCH: 0x0047f840 FUN_0047f840
int FUN_0047f840(int param_1, int param_2)
{
  int world;
  int cam;
  V3 grav;

  grav.x = 0.0f;
  grav.y = -9.8f;
  grav.z = 0.0f;
  if (DAT_006ce274 == 0) {
    memset(DAT_006c71d8, 0, sizeof(DAT_006c71d8));
    world = FUN_0047f4c0(1.0f);
    DAT_006ce274 = world;
    FUN_0055fe10(FUN_0055deb0(world), &grav);
    FUN_0055ad20(FUN_0055dec0(world), 1.0f);
    FUN_0055fe30(FUN_0055deb0(world), 0x19);
    FUN_0055fe40(FUN_0055deb0(world), 0x19);
    if (param_2 == 0) {
      param_2 = param_1;
    }
    cam = FUN_00562600(param_2, 0);
    FUN_0055c490(cam, FUN_0047f7d0);
    FUN_0055ddd0(world, cam, 0);
    FUN_0055c4f0(cam, 0.1f);
    FUN_0055c4a0(cam, 1.0f);
    FUN_0055bac0(cam, 1);
    DAT_0086caa0 = 0x7b;
    return world;
  }
  return DAT_006ce274;
}
