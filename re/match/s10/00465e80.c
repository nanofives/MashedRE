// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x00465e80: 100% (regs 100%) by hand from raw g2s 42.9%. Same shape as 0x00465ca0: 12-byte struct
   copy for the +0x58 vec3, float callee args 22050.0f / 1.0f, and the slot index in a fresh local (reusing
   param_3 for it loaded the table entry before param_3). */
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
typedef struct { undefined4 a, b, c; } Vec3_465e80;
MGLOBAL(int, DAT_00604570, 0x00604570);
MGLOBAL(int, DAT_0068f640, 0x0068f640);
void FUN_0045e0f0(int p0, float p1);
int FUN_00492d10(void);
void FUN_005a66d0(int p0, int p1);
uint FUN_005a6d60(int p0, int p1, int p2, undefined4 *p3);
void FUN_005a6dc0(int p0, int p1, int p2, float p3);
// MATCH: 0x00465e80 FUN_00465e80
void FUN_00465e80(int param_1,undefined4 *param_2,int param_3)
{
  int iVar1;
  int iVar2;

  if (FUN_00492d10() == 3) {
    iVar2 = (&DAT_00604570)[param_1 * 0x25] + param_3;
    iVar1 = (&DAT_0068f640)[iVar2 * 2];
    if ((*(byte *)(iVar1 + 0x34) & 2) == 0) {
      *(Vec3_465e80 *)(iVar1 + 0x58) = *(Vec3_465e80 *)param_2;
      if (*(int *)((&DAT_0068f640)[iVar2 * 2] + 0x30) != 0) {
        FUN_005a6d60(*(int *)((&DAT_0068f640)[iVar2 * 2] + 0x30),0,1,param_2);
      }
      FUN_005a66d0((&DAT_0068f640)[iVar2 * 2],1);
      *(undefined4 *)((&DAT_0068f640)[iVar2 * 2] + 0x3c) = 0x46ac4400;
      if (*(int *)((&DAT_0068f640)[iVar2 * 2] + 0x30) != 0) {
        FUN_005a6dc0(*(int *)((&DAT_0068f640)[iVar2 * 2] + 0x30),4,1,22050.0f);
      }
      FUN_0045e0f0(iVar2,1.0f);
    }
  }
}
