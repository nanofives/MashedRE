// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 h6 0x004924f0: 71.8% (regs 74.8%). Hand: one struct at 0x007f0a40 (opt[13][12], ctl[13][5], b/c/d[13], on[13][12]: the original addresses b, c and d from one register with -0x34/+0x34, and opt ends where ctl starts), memset intrinsic for the 0xdce9-dword clear (moved below the first zero store: the permuter's swap, it gives ebp as the zero register), opt[i][j] in the switch (opt[i][<const>] 55%). Tried: separate arrays (38.8%, the compiler turned the 13-loops into rep stosd), 1D indexing, i==0 / !i, i!=13, the permuter twice. Residual (b): the original removes i completely (pointer compares ecx < 0x7f0cc4 / 0x7f0d00 for i<1 / i<4, b/c/d through a walking pointer); ours keeps i in ecx and indexes b/c/d by i*4. */
/* ---- MATCH-S15 preamble (same text in every re/match/s15 source; not part of any function body) ----
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
#ifdef __cplusplus
extern "C"
#endif
void * __cdecl memset(void *, int, unsigned int);
#pragma intrinsic(memset)
typedef struct {
  int f0;
  int f4;
  int pad[17];
} S7f1058;
typedef struct {
  int opt[13][12];
  int ctl[13][5];
  int b[13];
  int c[13];
  float d[13];
  unsigned char on[13][12];
} Cfg7f0a40;
MGLOBALA(int, DAT_007f0f60, 0xdce9, 0x007f0f60);
MGLOBAL(int, DAT_007f0f2c, 0x007f0f2c);
MGLOBAL(int, DAT_007f0f30, 0x007f0f30);
MGLOBAL(int, DAT_007f0f34, 0x007f0f34);
MGLOBAL(int, DAT_007f0f38, 0x007f0f38);
MGLOBAL(int, DAT_007f0f3c, 0x007f0f3c);
MGLOBAL(int, DAT_007f0fec, 0x007f0fec);
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);
MGLOBAL(int, DAT_007f0ff0, 0x007f0ff0);
MGLOBAL(float, DAT_00803344, 0x00803344);
MGLOBALA(S7f1058, DAT_007f1058, 8, 0x007f1058);
MGLOBAL(Cfg7f0a40, DAT_007f0a40, 0x007f0a40);
void FUN_00431ae0(void);
void FUN_00431af0(void);
void FUN_00431b00(void);
void FUN_00431b10(void);
void FUN_00431d00(void);
void FUN_0045b350(void);
// MATCH: 0x004924f0 FUN_004924f0
int FUN_004924f0(void)
{
  int i;
  int j;
  DAT_007f0f2c = 0;
  memset(DAT_007f0f60, 0, sizeof(DAT_007f0f60));
  DAT_007f0f30 = 0;
  DAT_007f0f34 = 0;
  DAT_007f0f38 = 0;
  DAT_007f0f3c = 0;
  DAT_007f0fec = 0;
  DAT_007f0fd0 = 0;
  FUN_00431ae0();
  FUN_00431af0();
  FUN_00431b00();
  FUN_0045b350();
  FUN_00431b10();
  for (i = 0; i < 8; i++) {
    DAT_007f1058[i].f4 = 1;
  }
  for (i = 0; i < 13; i++) {
    for (j = 0; j < 5; j++) {
      DAT_007f0a40.ctl[i][j] = -1;
    }
    DAT_007f0a40.b[i] = 9;
    DAT_007f0a40.c[i] = 0x3b;
    DAT_007f0a40.d[i] = 0.99f;
    for (j = 0; j < 12; j++) {
      DAT_007f0a40.on[i][j] = 1;
      switch (j) {
      case 0:
        DAT_007f0a40.opt[i][j] = 0;
        break;
      case 1:
        DAT_007f0a40.opt[i][j] = i < 1;
        break;
      case 2:
        DAT_007f0a40.opt[i][j] = 0;
        break;
      case 3:
        DAT_007f0a40.opt[i][j] = i < 4 ? 2 : 0;
        break;
      case 4:
        DAT_007f0a40.opt[i][j] = 2;
        break;
      case 5:
        DAT_007f0a40.opt[i][j] = 0;
        break;
      case 6:
        DAT_007f0a40.opt[i][j] = 0;
        break;
      case 7:
        DAT_007f0a40.opt[i][j] = 0;
        break;
      case 8:
        DAT_007f0a40.opt[i][j] = 0;
        break;
      case 9:
        DAT_007f0a40.opt[i][j] = 0;
        break;
      case 10:
        DAT_007f0a40.opt[i][j] = 0;
        break;
      case 11:
        DAT_007f0a40.opt[i][j] = i < 1;
        break;
      }
    }
  }
  DAT_00803344 = 0.75f;
  DAT_007f0ff0 = 0;
  FUN_00431d00();
  return 1;
}
