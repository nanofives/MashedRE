// FLAGS: /O2 /GS /Ob1
/* TOPM01 0x004924f0: 100.0% (100/100 ins), --bytes BYTE-IDENTICAL (464 B, relocations masked), relocheck OK (37 checked). Started from re/match/s15/004924f0.c (71.8%). What closed it: (1) cases 1 and 3 of the switch written as if/else stores (if (i < 1) opt=1 else opt=0; if (i < 4) opt=2 else opt=0) instead of the value compares i < 1 / i < 4 ? 2 : 0. Probes showed this compiler rewrites an in-loop branch compare of i against the ctl row pointer (LFTR) but keeps i alive for a value compare (setl); with the branch form i dies, b/c/d get the walking pointer (edi -0x34/0/+0x34 at 0x00492583..0x00492590) and the compares become ecx < 0x7f0cc4 / 0x7f0d00 as in the original, the if/else is still emitted as setl / setge-dec-and. (2) memset of DAT_007f0f60 moved back above the DAT_007f0f2c = 0 store (rep stosd at 0x00492502 precedes the store at 0x00492504). Kept from s15: one struct at 0x007f0a40 (opt[13][12], ctl[13][5], b/c/d[13], on[13][12]), memset intrinsic, opt[i][j] in the switch. Tried without effect or worse: pointer IV for c in source (31.8-54.7%), unsigned/short i, while/do-while, __forceinline helpers, 2D bcd array, if (i == 0) (43.6%), store-0-then-if (46.0%), flags /G5 /G6 /G7 /Ox /Ob2 /O1 /TP. */
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
  memset(DAT_007f0f60, 0, sizeof(DAT_007f0f60));
  DAT_007f0f2c = 0;
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
        if (i < 1) {
          DAT_007f0a40.opt[i][j] = 1;
        } else {
          DAT_007f0a40.opt[i][j] = 0;
        }
        break;
      case 2:
        DAT_007f0a40.opt[i][j] = 0;
        break;
      case 3:
        if (i < 4) {
          DAT_007f0a40.opt[i][j] = 2;
        } else {
          DAT_007f0a40.opt[i][j] = 0;
        }
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
        if (i < 1) {
          DAT_007f0a40.opt[i][j] = 1;
        } else {
          DAT_007f0a40.opt[i][j] = 0;
        }
        break;
      }
    }
  }
  DAT_00803344 = 0.75f;
  DAT_007f0ff0 = 0;
  FUN_00431d00();
  return 1;
}
