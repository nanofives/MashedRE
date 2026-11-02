// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00448940: 100% by hand (h9): BYTE-IDENTICAL over the whole 880 B obj symbol (match.py --bytes,
   relocations masked; relocheck OK). match.py's instruction score reads 97.9% only because Ghidra's size (867 B)
   stops 13 B before the real end at 0x00448cb0 (push 0x28; call 0x4924c0; add esp, 0x1c; add esp, 0x24; ret).
   Raw 82.0%. What moved it: the bounding-sphere read written as RW's RpAtomicGetBoundingSphere shape (flag 2 at
   +0x4c -> FUN_004e5fc0, then &atomic->sphere at +0x1c) with member-wise copies (center, radius); the emitter
   vectors pos/vel and the last spawn position in two block scopes (the compiler overlaps them, frame 0x24, slot
   order v, vel, pos); FUN_004e45b0 (Ghidra RwFrameRemoveChild) and FUN_0049a0c0 (LAB_ passed as callback) by
   address; float literals for every .rdata constant (60.0f at 0x005cc728). */
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
typedef struct { float x, y, z; } MatchV3_00448940;
typedef struct { MatchV3_00448940 center; float radius; } MatchSphere_00448940;
MGLOBALA(int, DAT_008963e0, 25, 0x008963e0);
MGLOBAL(MatchSphere_00448940, DAT_008962c0, 0x008962c0);
MGLOBAL(int, DAT_0068324c, 0x0068324c);
MGLOBAL(int, DAT_00683248, 0x00683248);
MGLOBALA(int, DAT_008962e0, 64, 0x008962e0);
int FUN_0040bb30(const char *p0);
void FUN_00448770(int p0);
void FUN_00448820(int p0);
void FUN_00448880(int p0);
int FUN_004671a0(int p0);
int FUN_00484000(unsigned int p0, int p1, int p2, int p3);
int FUN_004840b0(unsigned int p0);
int FUN_00487e70(int p0, MatchV3_00448940 *p1, int p2, int p3);
int FUN_00491b20(MatchV3_00448940 *p0, float p1, MatchV3_00448940 *p2, MatchV3_00448940 *p3);
void FUN_00491c00(int p0);
void FUN_004924c0(int p0, int p1, int p2);
void FUN_00496d00(void (*p0)(void));
void FUN_00496d20(int p0, int p1);
void FUN_0049a0a0(int p0);
void FUN_0049a0c0(void);
void FUN_0049a0e0(void);
int FUN_004b3f90(int p0);
int FUN_004c1340(int p0, MatchV3_00448940 *p1, int p2);
void FUN_004c15c0(int p0);
int FUN_004c1b10(int p0, float p1);
void FUN_004e45b0(int p0, int p1);
void FUN_004e5fc0(int p0);
int FUN_004e6ab0(int p0);
// MATCH: 0x00448940 FUN_00448940
void FUN_00448940(int param_1)
{
  MatchV3_00448940 v;
  int atomic;
  MatchSphere_00448940 *sph;
  int i;
  int j;
  int n;

  FUN_0049a0e0();
  FUN_00496d00(FUN_0049a0c0);
  FUN_00496d20(*(int *)(param_1 + 0x105d4), 1);
  FUN_00484000(0, 0x80, 0x80, 0x600);
  FUN_0049a0a0(FUN_004840b0(0));
  FUN_00448770(param_1);
  FUN_00448880(param_1);
  FUN_00448820(param_1);
  FUN_004c15c0(*(int *)(*(int *)(param_1 + 0x10118) + 4));
  v.x = 0.0f;
  v.y = -4.1f;
  v.z = 0.0f;
  DAT_008963e0[0] = *(int *)(param_1 + 0x10118);
  atomic = FUN_004b3f90(DAT_008963e0[0]);
  sph = (*(unsigned char *)(atomic + 0x4c) & 2)
        ? (FUN_004e5fc0(atomic), (MatchSphere_00448940 *)(atomic + 0x1c))
        : (MatchSphere_00448940 *)(atomic + 0x1c);
  DAT_008962c0.center = sph->center;
  DAT_008962c0.radius = sph->radius;
  FUN_004e45b0(*(int *)(param_1 + 0x105d4), DAT_008963e0[0]);
  FUN_004c15c0(*(int *)(DAT_008963e0[0] + 4));
  for (i = 1; i < 25; i++) {
    DAT_008963e0[i] = FUN_004e6ab0(DAT_008963e0[0]);
    FUN_004c15c0(*(int *)(DAT_008963e0[i] + 4));
  }
  v.x = -150.0f;
  n = 0;
  for (i = 0; i < 5; i++) {
    v.z = -150.0f;
    for (j = 0; j < 5; j++) {
      FUN_004c1340(*(int *)(DAT_008963e0[n] + 4), &v, 0);
      v.z = v.z + 60.0f;
      n++;
    }
    v.x = v.x + 60.0f;
  }
  {
  MatchV3_00448940 pos;
  MatchV3_00448940 vel;
  pos.x = -46.74f;
  pos.y = 0.747f;
  pos.z = -70.071f;
  vel.x = -0.008f;
  vel.y = 0.02f;
  vel.z = -0.008f;
  v.x = 0.008f;
  v.y = 0.08f;
  v.z = 0.008f;
  FUN_00491c00(FUN_0040bb30("smoke"));
  FUN_00491b20(&pos, 0.01f, &vel, &v);
  }
  for (i = 0; i < *(int *)(param_1 + 0x10004); i++) {
    DAT_008962e0[DAT_0068324c++] = FUN_00487e70(0xc, (MatchV3_00448940 *)(param_1 + 0x1030 + i * 0x40), 0, 0x14);
  }
  for (i = 0; i < *(int *)(param_1 + 0x10008); i++) {
    DAT_008962e0[DAT_0068324c++] = FUN_00487e70(7, (MatchV3_00448940 *)(param_1 + 0x2030 + i * 0x40), 0, 0x14);
  }
  for (i = 0; i < *(int *)(param_1 + 0x1000c); i++) {
    DAT_008962e0[DAT_0068324c++] = FUN_00487e70(2, (MatchV3_00448940 *)(param_1 + 0x3030 + i * 0x40), 0, 0x14);
  }
  {
  MatchV3_00448940 pos2;
  pos2.x = -33.025f;
  pos2.y = 2.27f;
  pos2.z = -42.1f;
  DAT_00683248 = DAT_0068324c;
  DAT_008962e0[DAT_0068324c++] = FUN_00487e70(0xc, &pos2, 0, 0x14);
  }
  if (*(float *)(param_1 + 0x105f8) == *(float *)(param_1 + 0x105fc)) {
    FUN_004c1b10(FUN_004671a0(0), 800.0f);
    *(float *)(FUN_004671a0(0) + 0x88) = 200.0f;
    FUN_004671a0(0);
    FUN_004924c0(0x28, 0x2c, 0x30);
  }
}
