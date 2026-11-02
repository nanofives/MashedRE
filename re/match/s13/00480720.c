// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x00480720 (h9, by hand): 100% (regs 100%). Car respawn with speed clamp (car array 0x008815b4,
   stride 0xd04). Vertex record is {V3 p; pad; V3 n; pad} (stride 0x20). Literals: 300.0 @0x005cd8fc,
   5000.0 @0x005ce9b0, 1/300 @0x005cea6c; printf = 0x004a2cbd (CRT, unchecked by name). Residual: none. */
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
typedef struct V3 { float x, y, z; } V3;
typedef struct Mat { V3 right; int f0c; V3 up; int f1c; V3 at; int f2c; V3 pos; int f3c; } Mat;
typedef struct Vtx { V3 p; int f0c; V3 n; int f1c; } Vtx;  /* stride 0x20 */
typedef struct Car720 {
  int active;           /* +0x000 (0x008815b4) */
  int timer;            /* +0x004 */
  char pad08[0x11c];
  V3 off;               /* +0x124 (0x008816d8) */
  char pad130[0x86c];
  V3 pos;               /* +0x99c (0x00881f50) */
  V3 v9a8;              /* +0x9a8 (0x00881f5c) */
  char pad9b4[0x350];
} Car720;               /* stride 0xd04 */
MGLOBALA(Car720, DAT_008815b4, 16, 0x008815b4);
MGLOBALA(int, DAT_006c9a78, 16, 0x006c9a78);
int printf(const char *fmt, ...);
int FUN_0057c210(int p0);
int FUN_0055ad30(int p0, int p1);
void FUN_0055ac00(int p0, int p1, int p2);
void FUN_0055ae70(int p0, int p1, int p2);
void FUN_0046d4a0(Mat **p0, int p1);
float FUN_004c3ac0(V3 *p0);
void FUN_004c3df0(V3 *p0, V3 *p1, int p2, Mat *p3);
void FUN_004c45f0(Mat *p0);
void FUN_004c0e50(int p0);
void FUN_0055dff0(int p0, int p1);
// MATCH: 0x00480720 FUN_00480720
int FUN_00480720(int i)
{
  float speed;
  float carspeed;
  Mat *m;
  int obj;
  V3 v;
  V3 off;
  int *geo;
  Mat *dst;
  float scaled;

  if (i >= 16) {
    return 0;
  }
  if (DAT_008815b4[i].active != 0) {
    return 0;
  }
  FUN_0046d4a0(&m, i);
  obj = FUN_0057c210(DAT_006c9a78[i]);
  if (*(int *)(obj + 0x24) == 0 ||
      ((*(unsigned int **)(*(int *)(obj + 0x24) + 0x60))[*(unsigned short *)(obj + 0x20) >> 5] &
       (1 << (*(unsigned short *)(obj + 0x20) & 0x1f))) == 0) {
    FUN_0055ad30(*(int *)(obj + 0x24), obj);
    FUN_0055ac00(*(int *)(obj + 0x24), obj, 1);
  }
  FUN_0055ae70(*(int *)(obj + 0x24), obj, i + 2);
  geo = *(int **)(*(int *)(obj + 0x10) + 4);
  v.x = 0.0f;
  v.y = 0.0f;
  v.z = 0.0f;
  (*(Vtx **)(*(int *)(*geo + 0x10) + 8))[geo[1]].n = v;
  v = (*(Vtx **)(*(int *)(*geo + 0x10) + 8))[geo[1]].p;
  speed = FUN_004c3ac0(&v);
  carspeed = FUN_004c3ac0(&DAT_008815b4[i].pos);
  scaled = speed * 300.0f;
  if (scaled > 5000.0f) {
    v.x = DAT_008815b4[i].pos.x * 0.0033333334f;
    v.y = DAT_008815b4[i].pos.y * 0.0033333334f;
    v.z = DAT_008815b4[i].pos.z * 0.0033333334f;
    (*(Vtx **)(*(int *)(*geo + 0x10) + 8))[geo[1]].p = v;
    printf("RWP speed=%f (%f)\n", (double)speed, (double)scaled);
    printf("PW car speed=%f\n", (double)carspeed);
    printf("SPEED ADJUSTMENT OCCURRED!\n");
  }
  dst = *(Mat **)(*(int *)(obj + 0x10) + 8);
  v = dst->pos;
  *dst = *m;
  off = DAT_008815b4[i].off;
  FUN_004c3df0(&off, &off, 1, dst);
  dst->pos.x -= off.x;
  dst->pos.y -= off.y;
  dst->pos.z -= off.z;
  FUN_004c45f0(dst);
  FUN_004c0e50(*(int *)(DAT_006c9a78[i] + 4));
  FUN_0055dff0(obj, 0);
  FUN_0055ac00(*(int *)(obj + 0x24), obj, 1);
  DAT_008815b4[i].pos.x = DAT_008815b4[i].pos.y = DAT_008815b4[i].pos.z =
    DAT_008815b4[i].v9a8.x = DAT_008815b4[i].v9a8.y = DAT_008815b4[i].v9a8.z = 0.0f;
  DAT_008815b4[i].active = 1;
  DAT_008815b4[i].timer = 600;
  return 1;
}
