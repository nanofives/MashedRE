// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 h3 0x004862d0: BYTE-IDENTICAL whole symbol (126 B, match.py --bytes; the per-instruction 88.6% is
   Ghidra's 120-byte size stopping before the last call/add esp/pop/ret). Raw g2s 75.0%. Clears f70/f74/f78 of
   the 150 S7c records at 0x006fe790 (struct copied from h4's 0x004864f0 file; 0x00486370 fills them), loop
   'i = 150; do { i--; ... } while (i != 0)'. relocheck OK. */
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
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef struct V3 { float x, y, z; } V3;
typedef struct Mat { V3 right; unsigned int flags; V3 up; unsigned int pad1; V3 at; unsigned int pad2; V3 pos; unsigned int pad3; } Mat;
typedef struct S7c {
    V3 pos;
    int f0c, f10;
    float f14;
    float f18;
    int f1c, f20, f24, f28;
    float f2c;
    Mat m;
    int f70, f74, f78;
} S7c;

MGLOBALA(undefined4, DAT_006fe700, 16, 0x006fe700);
MGLOBALA(undefined4, DAT_006fe740, 20, 0x006fe740);
MGLOBALA(S7c, DAT_006fe790, 150, 0x006fe790);
MGLOBALA(char, s_exp_cloud2_005ce5f8, 11, 0x005ce5f8);
MGLOBALA(char, s_scorch_005ccca0, 7, 0x005ccca0);

int FUN_0040bb30(const char *name);
void FUN_004770c0(void *blk, unsigned int a, unsigned int b, int tex);
void FUN_00476cb0(void *blk, int a, int b);
void FUN_00476be0(void *blk, int a);

// MATCH: 0x004862d0 FUN_004862d0
void FUN_004862d0(void)
{
  int i;

  i = 150;
  do {
    i--;
    DAT_006fe790[i].f70 = 0;
    DAT_006fe790[i].f74 = 0;
    DAT_006fe790[i].f78 = 0;
  } while (i != 0);
  FUN_004770c0(DAT_006fe700, 0x427, 0xfa, FUN_0040bb30(s_exp_cloud2_005ce5f8));
  FUN_00476cb0(DAT_006fe700, 5, 2);
  FUN_00476be0(DAT_006fe700, 0);
  FUN_004770c0(DAT_006fe740, 0x80c, 0x96, FUN_0040bb30(s_scorch_005ccca0));
}
