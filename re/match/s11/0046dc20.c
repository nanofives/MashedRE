// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x0046dc20 (helper h4): 100% (regs 100%). Event ring DAT_007e9de0 as one struct (n, type/time/data[0x100], 8-dword and 16-dword payload rows). The payload is one uninitialized int b[16] (as in the original): the 8-dword row is written per field as {0xff0000ff, b[1..7]} (a struct copy gave rep movsd, 50%), then b[0..15] by loop. Vehicle record DAT_008815a0 +0x9f0 state. */
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
typedef struct { int d[8]; } Ev8_0046dc20;
typedef struct {
  unsigned int n;          /* +0x0000 */
  int type[0x100];         /* +0x0004 */
  int time[0x100];         /* +0x0404 */
  int data[0x100];         /* +0x0804 */
  Ev8_0046dc20 e8[0x100];  /* +0x0c04 */
  int e16[0x100][16];      /* +0x2c04 */
} EvQ_0046dc20;
typedef struct { char pad0[0x9f0]; int state; char pad9f4[0xd04 - 0x9f4]; } Car_0046dc20;
MGLOBALA(Car_0046dc20, DAT_008815a0, 16, 0x008815a0);
MGLOBAL(EvQ_0046dc20, DAT_007e9de0, 0x007e9de0);
MGLOBAL(int, DAT_007f1030, 0x007f1030);
int FUN_0040e350(void);
// MATCH: 0x0046dc20 FUN_0046dc20
void FUN_0046dc20(int param_1)
{
  int k, j;
  int t;
  int b[16];
  if (DAT_008815a0[param_1].state == 0 || DAT_008815a0[param_1].state == 1) {
    k = FUN_0040e350();
    t = DAT_007f1030;
    if (k != 1 && k != 2 && k != 3 && k != 4 && k != 5) {
      DAT_007e9de0.type[DAT_007e9de0.n] = 6;
      DAT_007e9de0.time[DAT_007e9de0.n] = t;
      DAT_007e9de0.data[DAT_007e9de0.n] = param_1 + 0xff00;
      DAT_007e9de0.e8[DAT_007e9de0.n].d[0] = 0xff0000ff;
      DAT_007e9de0.e8[DAT_007e9de0.n].d[1] = b[1];
      DAT_007e9de0.e8[DAT_007e9de0.n].d[2] = b[2];
      DAT_007e9de0.e8[DAT_007e9de0.n].d[3] = b[3];
      DAT_007e9de0.e8[DAT_007e9de0.n].d[4] = b[4];
      DAT_007e9de0.e8[DAT_007e9de0.n].d[5] = b[5];
      DAT_007e9de0.e8[DAT_007e9de0.n].d[6] = b[6];
      DAT_007e9de0.e8[DAT_007e9de0.n].d[7] = b[7];
      for (j = 0; j < 16; j++) DAT_007e9de0.e16[DAT_007e9de0.n][j] = b[j];
      DAT_007e9de0.n++;
      if (DAT_007e9de0.n >= 0x100) DAT_007e9de0.n = 0;
    }
  }
}
