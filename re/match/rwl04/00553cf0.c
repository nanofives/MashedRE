// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00553cf0 h1: best 40.9% (regs 72.0%, ops 87.1%); structure only (262 B, inlined clear of the 0x00553590 body plus list loop). Orig keeps param_1 in esi / list ptr in ebp with the return value reloaded from stack; ours allocates differently. Class (b). */
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
typedef int (*code1)(int);
typedef int (*code2)(int, int);
typedef struct { char pad[0x10c]; code1 fn10c; char pad2[0xc]; code2 fn11c; } RwGlob;
typedef struct Obj { int a; int b; int c; int d; int e; int f; } Obj;
typedef struct Top { int a; int p4; int c8; int cC; int p10; Obj *p14; struct Top *p18; } Top;
typedef struct Nd { int a; int b; float f2; float f3; } Nd;
typedef struct Ls { int a; int b; int c; int d; int e; struct Ls *next; } Ls;
MGLOBAL(Top *, DAT_00912a28, 0x00912a28);
MGLOBAL(int, DAT_00912a24, 0x00912a24);
MGLOBAL(RwGlob *, DAT_007d3ff8, 0x007d3ff8);
int FUN_00553620(Top *p0, Ls *p1);
int FUN_00553c50(void *p0, float *p1);
int FUN_00553e80(int p0);
void FUN_005c4ba0(int p0);
int FUN_005c4d30(int p0);
int FUN_005c4d50(int p0);
// MATCH: 0x00553cf0 FUN_00553cf0
Ls * FUN_00553cf0(Ls *param_1,float *param_2)
{
  float fVar1;
  Obj *piVar2;
  Top *puVar3;
  int iVar4;
  Ls *piVar5;
  
  iVar4 = param_1->a;
  if (param_1->b == -1) {
    iVar4 = FUN_005c4d30(iVar4);
  }
  fVar1 = *(float *)(iVar4 + 4);
  param_2[2] = fVar1;
  *param_2 = fVar1;
  fVar1 = *(float *)(iVar4 + 8);
  param_2[3] = fVar1;
  param_2[1] = fVar1;
  piVar5 = param_1;
  do {
    puVar3 = DAT_00912a28;
    if (piVar5->d == 0) {
      piVar2 = DAT_00912a28->p14;
      if (piVar2 != 0) {
        iVar4 = piVar2->f;
        if (piVar2->a != 0) {
          if (piVar2->b == -1) {
            FUN_005c4d50(piVar2->a);
          }
          else {
            DAT_007d3ff8->fn10c(piVar2->a);
          }
        }
        piVar2->a = 0;
        DAT_007d3ff8->fn11c(DAT_00912a24,(int)piVar2);
        FUN_00553e80(iVar4);
      }
      puVar3->p14 = 0;
      FUN_005c4ba0(puVar3->a);
      puVar3->c8 = 0;
      puVar3->cC = 1;
      puVar3->p18 = puVar3;
      puVar3 = DAT_00912a28;
      FUN_00553620(DAT_00912a28,piVar5);
      FUN_00553c50(puVar3,param_2);
    }
    else {
      FUN_00553c50(piVar5,param_2);
    }
    piVar5 = piVar5->next;
  } while (piVar5 != 0);
  param_2[2] = param_2[2] - *param_2;
  param_2[3] = param_2[3] - param_2[1];
  return param_1;
}
