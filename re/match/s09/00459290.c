// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00459290: 100% by hand (Ghidra size 277 stops before the epilogue: add esp,0x10; ret; match.py
   --bytes: 280 B whole symbol BYTE-IDENTICAL, relocations masked). Raw was 57.5%. What moved it: the 4-slot array
   is one 0x58-byte struct array based at 0x0068b9f8 (Ghidra's 0x0068ba30 / 0x0068bb90 are &slot[0].mat /
   &slot[4].mat), the matrix as a struct written through slot.mat each time, and `i = 4` set before the first call
   with a `do { i--; ... } while (i != 0)` loop. Callee arities from the call sites (FUN_00474d60 takes 2). */
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
typedef struct {
  float right[3]; unsigned int flags;
  float up[3]; unsigned int pad1;
  float at[3]; unsigned int pad2;
  float pos[3]; unsigned int pad3;
} Mat459290;
typedef struct {
  int idx;          /* +0x00 */
  int f04;
  int f08;          /* +0x08 */
  int f0c;
  int f10;          /* +0x10 */
  int f14[9];
  Mat459290 *mat;   /* +0x38 */
  int f3c[6];
  int clump;        /* +0x54 */
} Slot459290;
MGLOBALA(unsigned int, DAT_0068b9b8, 16, 0x0068b9b8);
MGLOBALA(Slot459290, DAT_0068b9f8, 4, 0x0068b9f8);
MGLOBAL(int, DAT_0068bb58, 0x0068bb58);
int FUN_0040bb30(const char *name);
void FUN_00474d60(int clump, int a2);
void FUN_00476c10(unsigned int *p, float *col);
void FUN_00476cb0(unsigned int *p, int a, int b);
void FUN_004770c0(unsigned int *p, unsigned int a, unsigned int b, int c);
int FUN_004b3bf0(const char *name, int a, int b);
int FUN_004b3d80(const char *name, int a, int b);
Mat459290 *FUN_004c57a0(void);
void FUN_004c5c80(int a);
// MATCH: 0x00459290 FUN_00459290
void FUN_00459290(void)
{
  int i;
  float col[4];
  int tex;

  tex = FUN_0040bb30("Headlight");
  col[0] = 0.0f;
  col[1] = 0.0f;
  col[2] = 1.0f;
  col[3] = 1.0f;
  i = 4;
  FUN_004770c0(DAT_0068b9b8, 0x807, 0x19, tex);
  FUN_00476cb0(DAT_0068b9b8, 5, 6);
  FUN_00476c10(DAT_0068b9b8, col);
  DAT_0068bb58 = FUN_004b3d80("TargetHUD.txd", 0, 0);
  FUN_004c5c80(0);
  do {
    i--;
    DAT_0068b9f8[i].mat = FUN_004c57a0();
    DAT_0068b9f8[i].mat->at[2] = 1.0f;
    DAT_0068b9f8[i].mat->up[1] = 1.0f;
    DAT_0068b9f8[i].mat->right[0] = 1.0f;
    DAT_0068b9f8[i].mat->up[0] = 0.0f;
    DAT_0068b9f8[i].mat->right[2] = 0.0f;
    DAT_0068b9f8[i].mat->right[1] = 0.0f;
    DAT_0068b9f8[i].mat->at[1] = 0.0f;
    DAT_0068b9f8[i].mat->at[0] = 0.0f;
    DAT_0068b9f8[i].mat->up[2] = 0.0f;
    DAT_0068b9f8[i].mat->pos[2] = 0.0f;
    DAT_0068b9f8[i].mat->pos[1] = 0.0f;
    DAT_0068b9f8[i].mat->pos[0] = 0.0f;
    DAT_0068b9f8[i].mat->flags |= 0x20003;
    DAT_0068b9f8[i].clump = FUN_004b3bf0("lazer.dff", 0, 0);
    FUN_00474d60(DAT_0068b9f8[i].clump, 0);
    DAT_0068b9f8[i].idx = i;
    DAT_0068b9f8[i].f08 = -1;
    DAT_0068b9f8[i].f10 = 0;
  } while (i != 0);
}
