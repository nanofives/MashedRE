// FLAGS: /O2 /Ob1 /G5
/* h3 0x00544a70 58.1% (regs 61.3%): class (b). Original keeps base=piVar1[2] in edi and folds [edi+ecx*4] with ecx=idx*5, then lea ecx,[edi+ecx*4]; our compiler computes idx*0x14 separately. Tried struct forms, int* idx*5, base local. */
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
typedef unsigned char undefined1;
typedef unsigned short undefined2;
typedef unsigned int undefined4;
MGLOBAL(int, DAT_007dc670, 0x007dc670);
MGLOBAL(float, _DAT_005cc320, 0x005cc320);
typedef struct SL { int z; short a; short b; float c; float d; struct SL *next; } SL;
typedef struct { int n; int u; SL *s; } HD;
// MATCH: 0x00544a70 FUN_00544a70
void FUN_00544a70(int param_1,int param_2,undefined2 param_3,undefined2 param_4,float param_5)
{
  int *piVar1;
  int iVar2;
  int iVar4;
  float fVar3;
  fVar3 = _DAT_005cc320 / param_5;
  piVar1 = (int *)(DAT_007dc670 + param_1);
  iVar4 = param_2 * 0x14;
  iVar2 = piVar1[2];
  *(undefined4 *)(iVar2 + iVar4) = 0;
  iVar2 = iVar2 + iVar4;
  *(undefined2 *)(iVar2 + 4) = param_3;
  *(undefined2 *)(iVar2 + 6) = param_4;
  *(float *)(iVar2 + 8) = param_5;
  *(float *)(iVar2 + 0xc) = fVar3;
  if (param_2 == *piVar1 + -1) {
    *(int *)(iVar2 + 0x10) = piVar1[2];
    return;
  }
  *(int *)(iVar2 + 0x10) = iVar2 + 0x14;
}
