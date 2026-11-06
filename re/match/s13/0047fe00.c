// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x0047fe00 (h8): 100% (regs 100%, ops 100%), 108/108 ins.
   Sibling of 0x0047fad0. Table at 0x0086cba0 as a 0x110-byte struct indexed directly (no pointer local), loop as
   `if (0 < n) do {} while`, `* 0.1f` literal (0x005cc56c), float arg calls. The square store goes to
   DAT_006c6870[old count] before the increment (same address as Ghidra's DAT_006c686c[new count]; the permuter
   found the order, the base moved to keep the address). */
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
  float f00;      /* +0x00 */
  int f04;        /* +0x04 */
  RwV3d pos;      /* +0x08 */
  int f14;        /* +0x14 */
  int f18;        /* +0x18 */
  int pad1c[2];
  int f24;        /* +0x24 */
  float f28;      /* +0x28 */
  int pad2c[57];
} Ent86cba0;      /* 0x110 bytes */
MGLOBALA(Ent86cba0, DAT_0086cba0, 1, 0x0086cba0);
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
void FUN_0055c540(int p0, float p1);
int FUN_0055dec0(int p0);
int FUN_0057c220(int p0, int p1);
int FUN_0057c300(int *p0);
int *FUN_0057c500(void);
void FUN_0057c550(int *p0);
// MATCH: 0x0047fe00 FUN_0047fe00
void FUN_0047fe00(int param_1,int param_2,int param_3,int param_4,int param_5,int param_6)
{
  int *p;
  int h;
  int r;
  int i;
  int n;

  p = FUN_0057c500();
  h = *p;
  *(RwV3d *)(p + 1) = DAT_0086cba0[param_5].pos;
  FUN_0055c540(h,DAT_0086cba0[param_5].f00 * 0.1f);
  FUN_0055c4f0(h,DAT_0086cba0[param_5].f14);
  FUN_0055c4a0(h,DAT_0086cba0[param_5].f18);
  FUN_0055b940(p,DAT_0086cba0[param_5].f04,0);
  i = 0;
  if (0 < param_1) do {
    r = FUN_0057c300(p);
    n = DAT_006c6eb0;
    DAT_006c6b90[n] = DAT_0086cba0[param_5].f24 + i;
    DAT_006c9758[n] = param_5;
    DAT_006c6eb8[n] = 1;
    DAT_006c71d8[DAT_006c6eb0] = FUN_004e69a0(param_6);
    FUN_004e7e30(DAT_006c71d8[DAT_006c6eb0],FUN_004c0b30());
    FUN_0057c220(DAT_006c71d8[DAT_006c6eb0],r);
    FUN_004e4380(param_4,DAT_006c71d8[DAT_006c6eb0]);
    *(unsigned int *)(r + 8) &= 0xfffffff3;
    FUN_00559c40(FUN_0055dec0(param_3),r,-1,2);
    DAT_006c6870[DAT_006c6eb0] = DAT_0086cba0[param_5].f28 * DAT_0086cba0[param_5].f28;
    DAT_006c6eb0++;
    i++;
  } while (i < param_1);
  FUN_0057c550(p);
}
