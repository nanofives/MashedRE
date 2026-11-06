// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 h3 0x00486270: 100.0% (regs 100%). Raw g2s 62.9%. BoneRec[4] at 0x006fde10, same loop shapes as
   0x00485d90. relocheck OK. */
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

void FUN_004894f0(void *pool);

// MATCH: 0x00486270 FUN_00486270
void FUN_00486270(void)
{
  int i;
  int j;
  BoneRec *r;

  i = 4;
  do {
    i--;
    r = &DAT_006fde10[i];
    r->f238 = 0;
    r->index = i;
    FUN_004894f0(r->pool);
    j = 7;
    while (j-- != 0) {
      r->f1c8[j] = 0;
      r->f1e4[j] = 0;
      r->f200[j] = 0;
      r->f21c[j] = 0;
      r->flag[j] = 0;
    }
  } while (i != 0);
}
