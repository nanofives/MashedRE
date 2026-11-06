// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x0046d7f0 (helper h4): 100% (regs 100%). Literal 160.0f (.rdata 0x005cea3c); slot table 0x007f1038 (stride 0x4c, byte +4) via DAT_007f1a14[i].slot (stride 0x10); vehicle record DAT_008815a0 +0xbf4 clamped 0..3000. */
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
typedef struct { float x, y, z; } V3_0046d7f0;
typedef struct { float m[16]; } Mat_0046d7f0;
typedef struct {
  undefined1 pad0[0xbf4];
  int hp; /* +0xbf4 */
  undefined1 padbf8[0x10c];
} Rec_0046d7f0;
MGLOBALA(Rec_0046d7f0, DAT_008815a0, 16, 0x008815a0);
typedef struct { int slot; int f4[3]; } CarIn_0046d7f0;
typedef struct { int f0; unsigned char b4; unsigned char pad5[0x4c - 5]; } Slot4c_0046d7f0;
MGLOBALA(CarIn_0046d7f0, DAT_007f1a14, 16, 0x007f1a14);
MGLOBALA(Slot4c_0046d7f0, DAT_007f1038, 8, 0x007f1038);
// MATCH: 0x0046d7f0 FUN_0046d7f0
int FUN_0046d7f0(int param_1,int param_2)
{
  if (param_1 >= 0x10) return 0;
  if ((float)DAT_007f1038[DAT_007f1a14[param_1].slot].b4 > 160.0f) {
    DAT_008815a0[param_1].hp += param_2 * 2;
  }
  DAT_008815a0[param_1].hp -= param_2;
  if (DAT_008815a0[param_1].hp > 3000) DAT_008815a0[param_1].hp = 3000;
  if (DAT_008815a0[param_1].hp < 0) DAT_008815a0[param_1].hp = 0;
  return 1;
}
