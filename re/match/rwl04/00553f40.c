// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00553f40 h1: 100%; nested if (alloc != 0) else return 0, memcpy intrinsic. */
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
typedef struct Nd { int a; int b; int c; int d; int e; struct Nd *next; } Nd;
typedef int (*code1)(int);
typedef struct { char pad[0x10c]; code1 fn10c; } RwGlob;
MGLOBAL(RwGlob *, DAT_007d3ff8, 0x007d3ff8);
int FUN_0055deb0(int p0);
int FUN_005c4ad0(int p0, unsigned int p1);
int FUN_005c4c60(int p0, int p1, int p2);
int FUN_005c4d30(int p0);
void *memcpy(void *, const void *, unsigned int);
#pragma intrinsic(memcpy)
// MATCH: 0x00553f40 FUN_00553f40
Nd * FUN_00553f40(Nd *param_1)
{
  Nd *piVar1;
  int iVar2;
  int iVar3;
  int puVar4;
  int src;
  
  piVar1 = param_1;
  if ((param_1 != 0) && (param_1->b != -1)) {
    iVar2 = FUN_005c4ad0(0x18,0x301a1);
    if (iVar2 != 0) {
    if (param_1->b == -1) {
      src = FUN_005c4d30(param_1->a);
    }
    else {
      src = param_1->a;
    }
    iVar3 = piVar1->b;
    if (iVar3 == -1) {
      iVar3 = FUN_0055deb0(piVar1->a);
    }
    if (iVar3 != 0) {
      puVar4 = FUN_005c4c60(iVar2,iVar3,0x301a1);
      if (puVar4 != 0) {
        memcpy((void *)puVar4, (void *)src, iVar3 * 0x18);
      }
      DAT_007d3ff8->fn10c(src);
    }
    piVar1->b = -1;
    piVar1->a = iVar2;
    FUN_00553f40(piVar1->next);
    } else {
      return 0;
    }
  }
  return piVar1;
}
