// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 h3 0x00486220: BYTE-IDENTICAL whole symbol (79 B, match.py --bytes; the per-instruction 79.2% is
   Ghidra's 70-byte size stopping before the tail RwRenderStateSet(8,1) call). Raw g2s 75.0%. BoneRec[4] at
   0x006fde10; loop 'i = 4; do { i--; ... } while (i != 0)' over rec[i].pool. DAT_007d3ff8 = RwGlobals*, slot +0x20.
   relocheck OK. */
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
typedef struct MVec3 {
  float x;
  float y;
  float z;
} MVec3;

typedef struct BoneRec {
  int index;
  void *pool;
  MVec3 v[7][5];
  int flag[7];
  int f1c8[7];
  undefined4 f1e4[7];
  undefined4 f200[7];
  int f21c[7];
  undefined4 f238;
} BoneRec;

MGLOBALA(BoneRec, DAT_006fde10, 4, 0x006fde10);

typedef void (*RenderStateSetFn)(int state, int value);
MGLOBAL(char *, DAT_007d3ff8, 0x007d3ff8);
void FUN_00489910(void *pool);

// MATCH: 0x00486220 FUN_00486220
void FUN_00486220(void)
{
  int i;

  (*(RenderStateSetFn *)(DAT_007d3ff8 + 0x20))(8, 0);
  i = 4;
  do {
    i--;
    FUN_00489910(DAT_006fde10[i].pool);
  } while (i != 0);
  (*(RenderStateSetFn *)(DAT_007d3ff8 + 0x20))(8, 1);
}
