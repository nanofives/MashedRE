// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 h1 0x005ad910 (97 B). Best 66.7% (regs 83.3%), residual class (a). orig copies param_3 to its own slot ([esp+0x10]) before &param_3 is pushed and uses jne-body/ret-first layout; param type variants, if/else forms, 5 flag sets did not move it (same early-ret layout as 0x005addd0). */
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
int FUN_005ad770(unsigned int p0, int * p1, int * p2, int p3, int p4);
// MATCH: 0x005ad910 FUN_005ad910
void FUN_005ad910(unsigned int param_1,int param_2,int param_3,int param_4,int param_5)
{
  int local_4 = 0;
  int iVar2 = FUN_005ad770(0x2000,&local_4,&param_3,param_4,param_5);
  if (iVar2 == 0) {
    return;
  } else if (param_2 != 0) {
    unsigned int u = *(unsigned int *)(param_2 + 0x14);
    *(int *)(iVar2 + 0x18) = param_2;
    *(unsigned int *)(iVar2 + 0x14) = *(unsigned int *)(iVar2 + 0x14) | u | 0x4000;
    return;
  } else {
    *(unsigned int *)(iVar2 + 0x14) = *(unsigned int *)(iVar2 + 0x14) | param_1;
    return;
  }
}
