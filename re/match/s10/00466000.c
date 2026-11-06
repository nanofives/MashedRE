// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x00466000: 100% (regs 100%) by hand from raw g2s 45.9%. Guard as early returns, slot index stored
   back into param_1 (the original keeps param_1 and the index in esi), vec3 struct copy, float clamp
   if/else-if, param_5 typed float (stored to +0x3c and passed to FUN_005a6dc0). */
/* ---- MATCH-S10 preamble (same text in every re/match/s10 source; not part of any function body) ----
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
typedef struct { undefined4 a, b, c; } Vec3_466000;
MGLOBAL(int, DAT_0068f4c8, 0x0068f4c8);
MGLOBAL(int, DAT_008aa254, 0x008aa254);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(int, DAT_00604570, 0x00604570);
MGLOBAL(int, DAT_0068f640, 0x0068f640);
void FUN_0045e0f0(int p0, float p1);
int FUN_00492d10(void);
uint FUN_005a6d60(int p0, int p1, int p2, undefined4 *p3);
void FUN_005a6dc0(int p0, int p1, int p2, float p3);
// MATCH: 0x00466000 FUN_00466000
void FUN_00466000(int param_1,undefined4 *param_2,int param_3,float param_4,float param_5)
{
  int iVar1;

  if (DAT_0068f4c8 != 0 && DAT_008aa254 > 1 && param_1 == 0x20) return;
  if (FUN_00492d10() != 3) return;
  param_1 = (&DAT_00604570)[param_1 * 0x25] + param_3;
  iVar1 = (&DAT_0068f640)[param_1 * 2];
  if ((*(byte *)(iVar1 + 0x34) & 2) != 0) {
    *(Vec3_466000 *)(iVar1 + 0x58) = *(Vec3_466000 *)param_2;
    if (*(int *)((&DAT_0068f640)[param_1 * 2] + 0x30) != 0) {
      FUN_005a6d60(*(int *)((&DAT_0068f640)[param_1 * 2] + 0x30),0,1,param_2);
    }
    if (param_4 > DAT_005cc320) {
      param_4 = 1.0f;
    }
    else if (param_4 < DAT_005d757c) {
      param_4 = 0.0f;
    }
    FUN_0045e0f0(param_1,param_4);
    *(float *)((&DAT_0068f640)[param_1 * 2] + 0x3c) = param_5;
    if (*(int *)((&DAT_0068f640)[param_1 * 2] + 0x30) != 0) {
      FUN_005a6dc0(*(int *)((&DAT_0068f640)[param_1 * 2] + 0x30),4,1,param_5);
    }
  }
}
