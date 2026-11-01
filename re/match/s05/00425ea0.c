// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00425ea0 (47 B), h9 by hand: 100.0% (regs 100.0%). Raw 52.6%. Same 8 x 0x4c table (MGLOBALA at
   0x00899260): int index loop, FUN_004e6e00 (RpClumpDestroy) on +0, then intrinsic memset of the record
   (rep stosd 0x13). A pointer loop gives 94.7%. */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
  void *clump;     /* +0x00 */
  void *body;      /* +0x04 */
  void *part0;     /* +0x08 */
  void *part1;     /* +0x0c */
  int pad10[12];   /* +0x10 */
  float time;      /* +0x40 */
  void *anim;      /* +0x44 */
  int pad48;       /* +0x48 */
} AnimObj;
MGLOBALA(AnimObj, DAT_00899260, 8, 0x00899260);
void *memset(void *d, int c, unsigned int n);
#pragma intrinsic(memset)
int FUN_004e6e00(void *clump);
// MATCH: 0x00425ea0 FUN_00425ea0
void FUN_00425ea0(void)
{
  int i;

  for (i = 0; i < 8; i++) {
    if (DAT_00899260[i].clump != 0) {
      FUN_004e6e00(DAT_00899260[i].clump);
    }
    memset(&DAT_00899260[i], 0, sizeof(AnimObj));
  }
}