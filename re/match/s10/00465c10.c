// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x00465c10: 100% (regs 100%). Raw g2s was 97.4%: the only change is the second argument of
   FUN_0045e0f0 (float) written as 1.0f instead of the int 0x3f800000 (which converted to 1065353216.0f). */
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
MGLOBAL(undefined4, DAT_00604570, 0x00604570);
MGLOBAL(undefined4, DAT_0068f640, 0x0068f640);
void FUN_0045daf0(int p0, undefined4 p1);
void FUN_0045e0f0(int p0, float p1);
undefined4 FUN_00492d10(void);
void FUN_005a66d0(int p0, int p1);
void FUN_005a6dc0(int p0, int p1, int p2, float p3);
// MATCH: 0x00465c10 FUN_00465c10
void FUN_00465c10(int param_1,int param_2)

{
  int iVar1;
  
  iVar1 = FUN_00492d10();
  if ((iVar1 == 3) &&
     (iVar1 = (&DAT_00604570)[param_1 * 0x25] + param_2,
     (*(byte *)((&DAT_0068f640)[iVar1 * 2] + 0x34) & 2) == 0)) {
    FUN_0045daf0(iVar1,param_2);
    FUN_005a66d0((&DAT_0068f640)[iVar1 * 2],1);
    *(undefined4 *)((&DAT_0068f640)[iVar1 * 2] + 0x3c) = 0x46ac4400;
    if (*(int *)((&DAT_0068f640)[iVar1 * 2] + 0x30) != 0) {
      FUN_005a6dc0(*(int *)((&DAT_0068f640)[iVar1 * 2] + 0x30),4,1,22050.0f);
    }
    FUN_0045e0f0(iVar1,1.0f);
  }
  return;
}
