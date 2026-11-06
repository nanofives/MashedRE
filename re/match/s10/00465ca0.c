// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x00465ca0: 100% (regs 100%) by hand from raw g2s 46.6%. What moved it: the three dword stores from
   param_2 to +0x58/+0x5c/+0x60 are one 12-byte struct copy (add ecx,0x58 / mov eax,edi form); the guard written
   as early returns; float callee args 22050.0f / 1.0f; DAT_008aa2d0 is an int (inc dword ptr). */
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
typedef struct { undefined4 a, b, c; } Vec3_465ca0;
MGLOBAL(int, DAT_0068f4c8, 0x0068f4c8);
MGLOBAL(int, DAT_008aa254, 0x008aa254);
MGLOBAL(int, DAT_008aa2d0, 0x008aa2d0);
MGLOBAL(int, DAT_00604570, 0x00604570);
MGLOBAL(int, DAT_00604574, 0x00604574);
MGLOBAL(int, DAT_0068f640, 0x0068f640);
void FUN_0045e0f0(int p0, float p1);
int FUN_00492d10(void);
void FUN_005a66d0(int p0, int p1);
uint FUN_005a6d60(int p0, int p1, int p2, undefined4 *p3);
void FUN_005a6dc0(int p0, int p1, int p2, float p3);
// MATCH: 0x00465ca0 FUN_00465ca0
void FUN_00465ca0(int param_1,undefined4 *param_2)
{
  int iVar1;
  int iVar2;
  int iVar3;

  if (DAT_0068f4c8 != 0 && DAT_008aa254 > 1 && param_1 == 0x20) return;
  if (FUN_00492d10() != 3) return;
  iVar2 = (&DAT_00604570)[param_1 * 0x25];
  for (iVar3 = (&DAT_00604574)[param_1 * 0x25]; iVar3 > 0; iVar3--) {
    iVar1 = (&DAT_0068f640)[iVar2 * 2];
    if ((*(byte *)(iVar1 + 0x34) & 2) == 0) {
      *(Vec3_465ca0 *)(iVar1 + 0x58) = *(Vec3_465ca0 *)param_2;
      if (*(int *)((&DAT_0068f640)[iVar2 * 2] + 0x30) != 0) {
        FUN_005a6d60(*(int *)((&DAT_0068f640)[iVar2 * 2] + 0x30),0,1,param_2);
      }
      FUN_005a66d0((&DAT_0068f640)[iVar2 * 2],1);
      *(undefined4 *)((&DAT_0068f640)[iVar2 * 2] + 0x3c) = 0x46ac4400;
      if (*(int *)((&DAT_0068f640)[iVar2 * 2] + 0x30) != 0) {
        FUN_005a6dc0(*(int *)((&DAT_0068f640)[iVar2 * 2] + 0x30),4,1,22050.0f);
      }
      FUN_0045e0f0(iVar2,1.0f);
      iVar3 = 0;
    }
    iVar2++;
  }
  if (param_1 == 0x1c) {
    DAT_008aa2d0++;
  }
}
