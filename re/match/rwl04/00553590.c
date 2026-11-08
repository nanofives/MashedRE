// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00553590 h1: 100%; typed structs for vtable-style globals, 1-arg FUN_005c4d50. */
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
MGLOBAL(Top *, DAT_00912a28, 0x00912a28);
MGLOBAL(int, DAT_00912a24, 0x00912a24);
MGLOBAL(RwGlob *, DAT_007d3ff8, 0x007d3ff8);
int FUN_00553e80(int p0);
void FUN_005c4ba0(int p0);
int FUN_005c4d50(int p0);
// MATCH: 0x00553590 FUN_00553590
Top * FUN_00553590(void)
{
  Obj *piVar1;
  int iVar2;
  Top *puVar3;
  
  puVar3 = DAT_00912a28;
  piVar1 = puVar3->p14;
  if (piVar1 != 0) {
    iVar2 = piVar1->f;
    if (piVar1->a != 0) {
      if (piVar1->b == -1) {
        FUN_005c4d50(piVar1->a);
      }
      else {
        DAT_007d3ff8->fn10c(piVar1->a);
      }
    }
    piVar1->a = 0;
    DAT_007d3ff8->fn11c(DAT_00912a24,(int)piVar1);
    FUN_00553e80(iVar2);
  }
  puVar3->p14 = 0;
  FUN_005c4ba0(puVar3->a);
  puVar3->c8 = 0;
  puVar3->cC = 1;
  puVar3->p18 = puVar3;
  return DAT_00912a28;
}
