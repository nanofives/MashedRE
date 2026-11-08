// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00544bf0 h1: best 74.3% (regs 77.1%, ops 92.4%). Residue: f5c copy done in int regs (orig fld/fstp), error block/frame placement (orig keeps sub esp,8 first, push edi after je). Class (b). */
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
typedef struct E { int p0; int p4; int p8; } E;
typedef struct Ob {
  char pad0[4]; int x4; char pad8[0x10]; int x18; char pad1c[0x30]; unsigned int f4c;
  short s50; short s52; float f54; float f58; float f5c;
} Ob;
typedef struct Sl { int *a; float p4; int p8; } Sl;
MGLOBAL(int, DAT_007dc66c, 0x007dc66c);
MGLOBAL(int, DAT_007dc670, 0x007dc670);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
void FUN_004c0e50(int p0);
int FUN_004d7ff0(int p0);
int FUN_004d8480(int *p0);
#define DIRTY(o) do { (o)->f4c |= 3; if ((((o)->f4c & 4) == 0) && ((o)->x4 != 0)) { FUN_004c0e50((o)->x4); } } while (0)
// MATCH: 0x00544bf0 FUN_00544bf0
Ob * FUN_00544bf0(Ob *param_1,int param_2)
{
  int piVar1;
  int iVar3;
  int iVar5;
  int local_8;
  int local_4;
  float f;
  
  iVar3 = param_1->x18;
  if (iVar3 != 0) {
  piVar1 = DAT_007dc66c + (int)param_1;
  if ((*(int *)(DAT_007dc66c + 8 + (int)param_1) != iVar3) || (iVar5 = *(int *)piVar1, iVar5 == 0)) {
    iVar5 = *(int *)(DAT_007dc670 + 8 + iVar3);
    *(int *)(piVar1 + 8) = iVar3;
    *(int *)piVar1 = iVar5;
    if (iVar5 == 0) {
      return param_1;
    }
  }
  *(int *)(piVar1 + 4) = param_2;
  param_1->s50 = *(short *)(iVar5 + 4);
  DIRTY(param_1);
  param_1->s52 = *(short *)(*(int *)piVar1 + 6);
  DIRTY(param_1);
  f = *(float *)(piVar1 + 4);
  param_1->f5c = f;
  DIRTY(param_1);
  f = *(float *)(*(int *)piVar1 + 8);
  param_1->f54 = f;
  param_1->f58 = DAT_005cc320 / f;
  DIRTY(param_1);
  return param_1;
  }
  local_8 = 0x105;
  local_4 = FUN_004d7ff0(1);
  FUN_004d8480(&local_8);
  return 0;
}
