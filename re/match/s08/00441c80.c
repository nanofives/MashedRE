// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00441c80: 100% by hand (raw did not compile). First position as three float scalars (a struct
   keeps it contiguous and costs a frame dword), difference as a local struct d = v - pos, then out = v - d * 0.5f
   (0.5f is the .rdata literal at 0x005cc32c). The two ids are filled by FUN_0040e180 and overwritten by a chained
   assignment. */
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
typedef struct { float x, y, z; } V3_00441c80;
typedef struct {
  V3_00441c80 right; unsigned int flags;
  V3_00441c80 up; unsigned int pad1;
  V3_00441c80 at; unsigned int pad2;
  V3_00441c80 pos; unsigned int pad3;
} Mtx_00441c80;
void FUN_0040e180(int *p0, int *p1);
int FUN_0046d4a0(Mtx_00441c80 **p0, int p1);
// MATCH: 0x00441c80 FUN_00441c80
void FUN_00441c80(V3_00441c80 *param_1, int param_2)
{
  Mtx_00441c80 *m;
  int a;
  int b;
  float vx, vy, vz;
  V3_00441c80 d;

  FUN_0040e180(&a, &b);
  if (*(int *)(param_2 + 0x1c) != 0) {
    a = b = *(int *)(param_2 + 0x20);
  }
  FUN_0046d4a0(&m, a);
  vx = m->pos.x;
  vy = m->pos.y;
  vz = m->pos.z;
  FUN_0046d4a0(&m, b);
  d.x = vx - m->pos.x;
  d.y = vy - m->pos.y;
  d.z = vz - m->pos.z;
  param_1->x = vx - d.x * 0.5f;
  param_1->y = vy - d.y * 0.5f;
  param_1->z = vz - d.z * 0.5f;
}
