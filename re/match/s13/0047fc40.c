// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x0047fc40 (h8): 100% (regs 100%, ops 100%), 123/123 ins. Sibling of 0x0047fe00; table struct based at 0x0086cb84 (scaled RwV3d
   at +0x00 passed by address to 0x0055c810, scaled float +0x38 to 0x0055bab0). */
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
typedef struct { float x, y, z; } RwV3d;
typedef struct {
  RwV3d rot;      /* +0x00 0x0086cb84 */
  int pad0c[2];
  float fm8;      /* +0x14 0x0086cb98 */
  float fm4;      /* +0x18 */
  float f00;      /* +0x1c 0x0086cba0 */
  int f04;        /* +0x20 */
  RwV3d pos;      /* +0x24 0x0086cba8 */
  int f14;        /* +0x30 */
  int f18;        /* +0x34 */
  float f38;      /* +0x38 0x0086cbbc */
  int pad3c;
  int f24;        /* +0x40 0x0086cbc4 */
  float f28;      /* +0x44 0x0086cbc8 */
  int pad48[50];
} Ent86cb84;      /* 0x110 bytes */
MGLOBALA(Ent86cb84, DAT_0086cb84, 1, 0x0086cb84);
MGLOBAL(int, DAT_006c6eb0, 0x006c6eb0);
MGLOBALA(float, DAT_006c6870, 1, 0x006c6870);
MGLOBALA(int, DAT_006c6b90, 1, 0x006c6b90);
MGLOBALA(int, DAT_006c6eb8, 1, 0x006c6eb8);
MGLOBALA(int, DAT_006c71d8, 1, 0x006c71d8);
MGLOBALA(int, DAT_006c9758, 1, 0x006c9758);
int FUN_004c0b30(void);
void FUN_004e4380(int p0, int p1);
int FUN_004e69a0(int p0);
int FUN_004e7e30(int p0, int p1);
int FUN_00559c40(int p0, int p1, int p2, int p3);
float FUN_0055b940(int *p0, int p1, int p2);
void FUN_0055c4a0(int p0, int p1);
void FUN_0055c4f0(int p0, int p1);
void FUN_0055c810(int p0, RwV3d *p1);
void FUN_0055bab0(int p0, float p1);
int FUN_0055dec0(int p0);
int FUN_0057c220(int p0, int p1);
int FUN_0057c300(int *p0);
int *FUN_0057c500(void);
void FUN_0057c550(int *p0);
// MATCH: 0x0047fc40 FUN_0047fc40
void FUN_0047fc40(int param_1,int param_2,int param_3,int param_4,int param_5,int param_6)
{
  int *p;
  int h;
  int r;
  int i;
  int n;
  RwV3d v;

  p = FUN_0057c500();
  h = *p;
  *(RwV3d *)(p + 1) = DAT_0086cb84[param_5].pos;
  v.x = DAT_0086cb84[param_5].rot.x * 0.1f;
  v.y = DAT_0086cb84[param_5].rot.y * 0.1f;
  v.z = DAT_0086cb84[param_5].rot.z * 0.1f;
  FUN_0055c810(h,&v);
  FUN_0055c4f0(h,DAT_0086cb84[param_5].f14);
  FUN_0055c4a0(h,DAT_0086cb84[param_5].f18);
  FUN_0055bab0(h,DAT_0086cb84[param_5].f38 * 0.1f);
  FUN_0055b940(p,DAT_0086cb84[param_5].f04,0);
  i = 0;
  if (0 < param_1) do {
    r = FUN_0057c300(p);
    n = DAT_006c6eb0;
    DAT_006c6b90[n] = DAT_0086cb84[param_5].f24 + i;
    DAT_006c9758[n] = param_5;
    DAT_006c6eb8[n] = 1;
    DAT_006c71d8[DAT_006c6eb0] = FUN_004e69a0(param_6);
    FUN_004e7e30(DAT_006c71d8[DAT_006c6eb0],FUN_004c0b30());
    FUN_0057c220(DAT_006c71d8[DAT_006c6eb0],r);
    FUN_004e4380(param_4,DAT_006c71d8[DAT_006c6eb0]);
    *(unsigned int *)(r + 8) &= 0xfffffff3;
    FUN_00559c40(FUN_0055dec0(param_3),r,-1,2);
    DAT_006c6870[DAT_006c6eb0] = DAT_0086cb84[param_5].f28 * DAT_0086cb84[param_5].f28;
    DAT_006c6eb0++;
    i++;
  } while (i < param_1);
  FUN_0057c550(p);
}
