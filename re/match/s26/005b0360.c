// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 h1 0x005b0360 (121 B). Best 100.0% (regs 100.0%), exact. explicit i=0; if(i<n) do{}while(i<n) form (jbe). */
/* ---- MATCH-S11 preamble (same text in every re/match/s11 source; not part of any function body) ----
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
int FUN_005b03e0(int p0, unsigned int p1);
int FUN_005b0a90(int * p0, int p1, int p2, unsigned int p3, int p4, unsigned int p5, int p6);
void FUN_005b0b40(int * p0);
// MATCH: 0x005b0360 FUN_005b0360
int FUN_005b0360(int param_1,int param_2,int param_3,int *param_4,int param_5)
{
  unsigned int i;
  int iVar2 = *(int *)(param_1 + 0xc);
  int off;
  i = 0;
  if (i < *(unsigned int *)(iVar2 + 0x14)) {
    off = 0;
    do {
      unsigned int uVar1 = *(unsigned int *)(*(int *)(iVar2 + 0x18) + 0x14 + off);
      iVar2 = FUN_005b03e0(param_5,uVar1);
      if (iVar2 != 0) {
        iVar2 = FUN_005b0a90(param_4,iVar2,param_2,uVar1,0,i,param_3);
        if (iVar2 == 0) {
          FUN_005b0b40(param_4);
          return 0;
        }
      }
      i++;
      off += 0x28;
      iVar2 = *(int *)(param_1 + 0xc);
    } while (i < *(unsigned int *)(iVar2 + 0x14));
  }
  return 1;
}
