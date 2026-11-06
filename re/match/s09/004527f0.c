// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x004527f0 (h1): 100% (regs 100%). Raw converter 48.8%: struct-typed level object (offsets to 0x105d0, 0x40-byte tree records at +0x2000/+0x3000), each FUN_004b3f90 result in a local before its FUN_00479f30 (nested calls pop early), index loops, matrix struct copies, literal 0.75f, strings as MGLOBALA, printf at 0x004a2cbd by its FUN_ name. 98.6% until DAT_00684e30, DAT_005f9790 and DAT_00684c50 became file-static (pilot finding 2: the loads of param_1 fields are scheduled before the stores to these three, so they cannot alias): static under #ifndef __cplusplus, MGLOBAL under #else. Residual: none. */
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
typedef unsigned int undefined4;
typedef unsigned char undefined1;
typedef struct { float x, y, z; } V3_004527f0;
typedef struct { float m[16]; } M16_004527f0;
typedef struct { char a0[0x20]; V3_004527f0 v20; float f2c; V3_004527f0 pos; float f3c; } T40_004527f0;
typedef struct {
  char a0[0x30];
  V3_004527f0 v30;              /* 0x30 */
  char a3c[0x1000 - 0x3c];
  char a1000[0x1000];           /* 0x1000 */
  T40_004527f0 ta[64];          /* 0x2000 */
  T40_004527f0 tb[64];          /* 0x3000 */
  char a4000[0x2000];           /* 0x4000 */
  char a6000[0xa000];           /* 0x6000 */
  int f10000, f10004, n10008, n1000c, f10010, f10014, f10018; /* 0x10000 */
  char a1001c[0xf8];
  int f10114, f10118, f1011c;   /* 0x10114 */
  char a10120[0x34];
  int f10154;                   /* 0x10154 */
  char a10158[8];
  int f10160, f10164, f10168;   /* 0x10160 */
  char a1016c[0x444];
  int f105b0;                   /* 0x105b0 */
  char a105b4[0x1c];
  int f105d0;                   /* 0x105d0 */
} S_004527f0;
MGLOBAL(int, DAT_00684e28, 0x00684e28);
MGLOBAL(int, DAT_00684e2c, 0x00684e2c);
#ifndef __cplusplus
static int DAT_00684e30;
#else
MGLOBAL(int, DAT_00684e30, 0x00684e30);
#endif
MGLOBAL(int, DAT_00684e34, 0x00684e34);
MGLOBALA(int, DAT_0088ff60, 8, 0x0088ff60);
MGLOBAL(int, DAT_0088ff44, 0x0088ff44);
MGLOBAL(int, DAT_0088ff00, 0x0088ff00);
MGLOBAL(int, DAT_005f978c, 0x005f978c);
#ifndef __cplusplus
static int DAT_005f9790;
#else
MGLOBAL(int, DAT_005f9790, 0x005f9790);
#endif

MGLOBAL(M16_004527f0, DAT_00684de8, 0x00684de8);
MGLOBAL(M16_004527f0, DAT_00684c58, 0x00684c58);
#ifndef __cplusplus
static int DAT_00684c50;
#else
MGLOBAL(int, DAT_00684c50, 0x00684c50);
#endif
MGLOBAL(float, DAT_006915fc, 0x006915fc);
MGLOBAL(undefined1, DAT_00684d18, 0x00684d18);
MGLOBALA(char, s_OakTreeSprite_005ce2f8, 14, 0x005ce2f8);
MGLOBALA(char, s_Created__d_trees_005ce3e8, 18, 0x005ce3e8);
MGLOBALA(char, s_water_005ce2d0, 6, 0x005ce2d0);
undefined4 FUN_00452170(undefined4 param_1);
int FUN_004a2cbd(char *p0, ...);
int FUN_004b3f90(int p0);
int FUN_00479f30(int p0, int p1, void *p2);
void FUN_00452730(S_004527f0 *p0);
int FUN_00452310(int p0, int p1);
int FUN_00452260(int p0, void *p1);
void FUN_00452720(S_004527f0 *p0);
int FUN_004c5c00(int p0, char *p1);
void FUN_0047c330(int p0);
float FUN_004c3ac0(V3_004527f0 *p0);
void FUN_0047c560(V3_004527f0 *p0, int p1, void *p2, float p3);
int FUN_0047ce40(int p0);
void FUN_00451760(void *p0, int p1, int p2);
void FUN_00451680(void *p0, void *p1);
M16_004527f0 *FUN_0047d150(int p0);
void FUN_0047d100(int p0, int p1);
int FUN_00475090(char *p0, int p1);
void FUN_004758d0(int p0, int p1);
// MATCH: 0x004527f0 FUN_004527f0
void FUN_004527f0(S_004527f0 *param_1)
{
  int t;
  int i;
  V3_004527f0 v;

  t = FUN_004b3f90(param_1->f1011c);
  DAT_00684e28 = FUN_00479f30(t,param_1->n10008,param_1->ta);
  DAT_00684e2c = FUN_00479f30(t,param_1->n1000c,param_1->tb);
  t = FUN_004b3f90(param_1->f10118);
  DAT_00684e30 = FUN_00479f30(t,param_1->f10000,param_1);
  t = FUN_004b3f90(param_1->f10168);
  DAT_00684e34 = FUN_00479f30(t,param_1->f10018,param_1->a6000);

  FUN_00452730(param_1);
  for (i = 0; i < 8; i++) {
    DAT_0088ff60[i] = FUN_00452310(param_1->f10154,0x10);
  }
  DAT_0088ff44 = FUN_00452260(param_1->f10010,param_1->a4000);
  FUN_00452720(param_1);
  v = param_1->v30;
  v.y = v.y + 0.75f;
  FUN_0047c330(FUN_004c5c00(param_1->f105d0,s_OakTreeSprite_005ce2f8));
  for (i = 0; i < param_1->n10008; i++) {
    v = param_1->ta[i].pos;
    FUN_0047c560(&v,param_1->f10004,param_1->a1000,FUN_004c3ac0(&param_1->ta[i].v20));
    DAT_0088ff00++;
  }
  for (i = 0; i < param_1->n1000c; i++) {
    v = param_1->tb[i].pos;
    FUN_0047c560(&v,param_1->f10004,param_1->a1000,FUN_004c3ac0(&param_1->tb[i].v20));
    DAT_0088ff00++;
  }
  FUN_004a2cbd(s_Created__d_trees_005ce3e8,DAT_0088ff00);
  DAT_005f978c = FUN_0047ce40(1000);
  DAT_005f9790 = FUN_0047ce40(2000);
  FUN_00451760(&DAT_00684d18,param_1->f10114,param_1->f105b0);
  FUN_00451680(&DAT_00684d18,(void *)FUN_00452170);
  DAT_00684de8 = *FUN_0047d150(DAT_005f978c);
  DAT_00684c58 = *FUN_0047d150(DAT_005f9790);
  FUN_0047d100(DAT_005f978c,0);
  FUN_0047d100(DAT_005f9790,0);
  DAT_00684c50 = FUN_00475090(s_water_005ce2d0,param_1->f105d0);
  t = FUN_004b3f90(param_1->f10164);
  DAT_006915fc = 60.0f;
  FUN_004758d0(param_1->f10160,t);
}
