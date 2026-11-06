// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00454170 (h3): 100% (regs 100%, whole symbol byte-identical). Raw form 81% (unaff_ESI). The record
   pointer arrives in ESI: FUN_00454170 is a file-static callee defined before its callers (pilot finding 1), here
   with the real caller FUN_00454790 (also 100%, its own MATCH line). The file holds both. Callers in the original:
   0x00454790, 0x004547c0, 0x004548a0, 0x00454350. Record = 0x44-byte entries of the table at 0x00688020 (8 of
   them). Field names are offsets only. */
/* ---- MATCH-S09 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
typedef struct V3 { float x; float y; float z; } V3;
typedef struct Mat40 { V3 f0; int fc; V3 f10; int f1c; V3 f20; int f2c; V3 f30; int f3c; } Mat40;
typedef struct Frame34 { int f0; int f4; int f8; int fc; Mat40 f10; } Frame34;
typedef struct Clump20 { int f0; int *f4; } Clump20;
typedef struct Rec44 { V3 f0; float fc; V3 f10; V3 f1c; int f28; int f2c; Clump20 *f30; Frame34 *f34;
  int *f38; float f3c; float f40; } Rec44;
MGLOBALA(Rec44, DAT_00688020, 8, 0x00688020);
int FUN_004c1210(Frame34 *p0);
void FUN_004c15c0(void *p0);
int FUN_004e43b0(int p0, Clump20 *p1);
int FUN_004e4440(Clump20 *p0);
// MATCH: 0x00454170 FUN_00454170
static void FUN_00454170(Rec44 *p)
{
  int iVar1;
  p->f2c = 0;
  p->f28 = 0;
  p->f3c = 0;
  p->f0.x = 0;
  p->f0.y = 0;
  p->f0.z = 0;
  p->fc = 0;
  p->f40 = 0;
  FUN_004c15c0(p->f34);
  FUN_004c15c0(p->f38);
  if (p->f34->f4 != 0) {
    FUN_004c1210(p->f34);
  }
  iVar1 = FUN_004e4440(p->f30);
  if (iVar1 != 0) {
    FUN_004e43b0(iVar1, p->f30);
  }
}
// MATCH: 0x00454790 FUN_00454790
void FUN_00454790(void)
{
  Rec44 *p;
  for (p = DAT_00688020; (int)p < (int)(DAT_00688020 + 8); p++) {
    if (p->f2c >= 2 && p->f2c <= 5) FUN_00454170(p);
  }
}
