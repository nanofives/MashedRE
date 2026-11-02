// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x0043d7c0 (pause menu, top-down map T07), hand-matched by h1 2026-11-01.
   Score: 99.6% instructions (regs 99.6%); the only listed difference is the `add esp, 8; ret` epilogue past
   Ghidra's size (it stops at `pop ebx`, 0x0043debd). `match.py --bytes`: 1844 B whole obj symbol,
   0 differing unmasked bytes -> BYTE-IDENTICAL. Counted as matched.
   Raw converter 30.1%. What moved it: real control-block / slot / table types (63%), the 0xff210000/0xff220000
   test as a two-case switch with the 0xff220000 case first (81%), switch on DAT_0067eac0 with case 11 placed
   before case 1 and case 12 a separate copy of case 2's body (a direct 12-entry jump table instead of a byte
   index table, 87%), the table walk written as `p = (int *)row; n = *p++; ids = p;` with `id = *p++` (99.6%:
   the register rotation in that loop and in the DAT_0067e930 call both followed from it).
   Callees: FUN_0042ae10 takes 1 stack arg (push edi / add esp, 4); all callees are cdecl (plain `ret`).
   Residual class: none. */
/* ---- MATCH-S08 preamble (same text in every re/match/s08 source; not part of any function body) ----
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
typedef struct { unsigned char b[0x4c]; } CtrlBlock;     /* 0x007f1038 + i*0x4c, byte +9 read here */
typedef struct { int a; int b; int c; int d; } Slot10;   /* 0x007f1a14 + i*0x10 */
typedef struct { int count; int ids[11]; } Tbl30;        /* FUN_00413f90() + i*0x30 */
MGLOBAL(int, DAT_0067eca4, 0x0067eca4);
MGLOBAL(int, DAT_0067f19c, 0x0067f19c);
MGLOBAL(int, DAT_0067f1a0, 0x0067f1a0);
MGLOBAL(int, DAT_0067f1a4, 0x0067f1a4);
MGLOBAL(int, DAT_0067eab0, 0x0067eab0);
MGLOBAL(int, DAT_0067eab8, 0x0067eab8);
MGLOBAL(int, DAT_0067eabc, 0x0067eabc);
MGLOBAL(char, DAT_0067eac6, 0x0067eac6);
MGLOBAL(char, DAT_0067eac4, 0x0067eac4);
MGLOBAL(char, DAT_0067eac5, 0x0067eac5);
MGLOBAL(int, DAT_0067eac0, 0x0067eac0);
MGLOBAL(int, DAT_0067f1a8, 0x0067f1a8);
MGLOBAL(int, DAT_0067ed6c, 0x0067ed6c);
MGLOBAL(int, DAT_0067e9fc, 0x0067e9fc);
MGLOBAL(int, DAT_0067e930, 0x0067e930);
MGLOBAL(int, DAT_0067e92c, 0x0067e92c);
MGLOBAL(int, DAT_0067e928, 0x0067e928);
MGLOBAL(int, DAT_0067e924, 0x0067e924);
MGLOBAL(int, DAT_0067e920, 0x0067e920);
MGLOBAL(int, DAT_0067e91c, 0x0067e91c);
MGLOBAL(int, DAT_0067e918, 0x0067e918);
MGLOBAL(int, DAT_0067e914, 0x0067e914);
MGLOBAL(int, DAT_0067ece4, 0x0067ece4);
MGLOBAL(int, DAT_0067ecf0, 0x0067ecf0);
MGLOBAL(int, DAT_0067ed04, 0x0067ed04);
MGLOBAL(int, DAT_0067ed00, 0x0067ed00);
MGLOBAL(int, DAT_0067ecfc, 0x0067ecfc);
MGLOBAL(int, DAT_0067ecf4, 0x0067ecf4);
MGLOBAL(int, DAT_0067ece8, 0x0067ece8);
MGLOBAL(int, DAT_0067ecb4, 0x0067ecb4);
MGLOBAL(int, DAT_0067ea64, 0x0067ea64);
MGLOBAL(int, DAT_0067ecdc, 0x0067ecdc);
MGLOBAL(int, DAT_0067ea70, 0x0067ea70);
MGLOBAL(int, DAT_0067f17c, 0x0067f17c);
MGLOBAL(int, DAT_0067ea84, 0x0067ea84);
MGLOBAL(int, DAT_0067eadc, 0x0067eadc);
MGLOBAL(int, DAT_0067ead8, 0x0067ead8);
MGLOBAL(float, DAT_007f1004, 0x007f1004);
MGLOBALA(int, DAT_0067ea10, 12, 0x0067ea10);
MGLOBALA(CtrlBlock, DAT_007f1038, 28, 0x007f1038);
MGLOBALA(Slot10, DAT_007f1a14, 4, 0x007f1a14);
void FUN_0040ab40(void);
void FUN_0040ac80(void);
void FUN_0040b810(void);
void FUN_0040de10(void);
void FUN_0040e360(int p0);
Tbl30 *FUN_00413f90(void);
void FUN_00422b30(void);
void FUN_004298c0(void);
void FUN_00429aa0(void);
int FUN_0042ae10(int p0);
int FUN_0042aeb0(void);
int FUN_0042af50(void);
void FUN_0042b950(void);
void FUN_0042bf30(int p0, int p1, int p2, int p3, int p4, int p5);
void FUN_0042c150(void);
void FUN_0042c220(void);
void FUN_0043d2a0(int p0, int p1);
void FUN_0046dc00(int p0, int p1);
void FUN_00493570(void);
void FUN_00493580(void);
__declspec(dllimport) void __stdcall PostQuitMessage(int p0);
// MATCH: 0x0043d7c0 FUN_0043d7c0
void FUN_0043d7c0(void)
{
  int state;
  int ev;
  int i;
  int j;
  int n;
  int id;
  int skip;
  int *ids;
  int *p;

  if (DAT_0067eca4 == 0) {
    DAT_0067f19c = 0;
    DAT_0067f1a0 = 0;
    DAT_0067f1a4 = 0;
  }
  state = DAT_0067eab0;
  if (state < 3 && state != 0) {
    DAT_0067eab8 += (int)(DAT_007f1004 * 1000.0f);
    if (DAT_0067eab8 > 0xff) {
      DAT_0067eab8 = 0xff;
    }
  }
  if (state == 2) {
    DAT_0067eac6 = 0;
    DAT_0067eac4 = 0;
    DAT_0067eac5 = 0;
    switch (DAT_0067eabc) {
    case -0xde0000:
      for (i = 0; i < 12; i++) {
        if (DAT_007f1038[i].b[9] != 0 && DAT_007f1038[i + 16].b[9] == 0) {
          DAT_0067eab0 = 3;
          DAT_0067eac6 = 1;
        }
      }
      break;
    case -0xdf0000:
      FUN_0042c220();
      for (i = 0; i < 12; i++) {
        if (DAT_0067ea10[i] != 0 && DAT_007f1038[i].b[9] != 0 && DAT_007f1038[i + 16].b[9] == 0) {
          DAT_0067ea10[i] = 0;
          if (i < 6) {
            DAT_0067ea10[i + 6] = 0;
          }
        }
      }
      FUN_0042c150();
      for (i = 0; i < 12; i++) {
        if (DAT_0067ea10[i] != 0) break;
      }
      if (i == 12) {
        DAT_0067eab0 = 3;
        DAT_0067eac6 = 1;
      }
      break;
    }
    switch (DAT_0067eac0) {
    case 11:
      if (FUN_0042ae10(0) != 0) {
        DAT_0067eab0 = 3;
        DAT_0067eac4 = 1;
        DAT_0067f19c = 1;
      }
      break;
    case 1:
      if (FUN_0042ae10(0) != 0 || FUN_0042aeb0() != 0) {
        DAT_0067eab0 = 3;
        DAT_0067eac4 = 1;
        DAT_0067f19c = 1;
      }
      break;
    case 2:
      if (FUN_0042ae10(0) != 0) {
        DAT_0067eab0 = 3;
        DAT_0067eac4 = 1;
        DAT_0067f19c = 1;
      }
      if (FUN_0042aeb0() != 0) {
        DAT_0067eab0 = 3;
        DAT_0067eac5 = 1;
        DAT_0067f1a0 = 1;
      }
      break;
    case 12:
      if (FUN_0042ae10(0) != 0) {
        DAT_0067eab0 = 3;
        DAT_0067eac4 = 1;
        DAT_0067f19c = 1;
      }
      if (FUN_0042aeb0() != 0) {
        DAT_0067eab0 = 3;
        DAT_0067eac5 = 1;
        DAT_0067f1a0 = 1;
      }
      break;
    case 7:
      if (FUN_0042af50() != 0) {
        DAT_0067eab0 = 3;
        DAT_0067eac6 = 1;
        DAT_0067f19c = 1;
      }
      break;
    case 9:
      if (FUN_0042ae10(0) != 0) {
        DAT_0067eab0 = 3;
        DAT_0067f19c = 1;
      }
      if (FUN_0042aeb0() != 0) {
        DAT_0067eadc++;
        if (DAT_0067eadc == DAT_0067ead8) {
          DAT_0067eadc = 0;
        }
        DAT_0067f1a0 = 1;
      }
      break;
    }
  }
  if (DAT_0067eab0 == 3) {
    DAT_0067eab8 += (int)(DAT_007f1004 * -1000.0f);
    if (DAT_0067eab8 >= 0) {
      return;
    }
    ev = DAT_0067eabc;
    DAT_0067eab8 = 0;
    DAT_0067eab0 = 0;
    DAT_0067eabc = 0;
    switch (ev) {
    case -0xe20000:
      if (DAT_0067eac4 != 0) {
        FUN_0043d2a0(0, 2);
        FUN_0040de10();
        DAT_0067e914 = 1;
        FUN_004298c0();
      }
      break;
    case -0xe10000:
      if (DAT_0067eac4 != 0) {
        FUN_00493570();
        DAT_0067f1a8 = 0;
        FUN_0043d2a0(7, 0);
        FUN_0040de10();
        if (DAT_0067ed6c != 0) {
          DAT_0067e9fc = 3;
        }
      }
      break;
    case -0xe00000:
      if (DAT_0067eac4 != 0) {
        FUN_00493580();
        FUN_0042b950();
        FUN_0043d2a0(1, 0);
        FUN_0040de10();
      }
      break;
    case -0xdf0000:
    case -0xde0000:
      if (DAT_0067ece4 > 0) {
        if (DAT_0067ecf0 != -1) {
          FUN_0042bf30(DAT_0067ece8, DAT_0067ecf0, DAT_0067ecf4, DAT_0067ecfc, DAT_0067ed00, DAT_0067ed04);
        }
        DAT_0067ecf0 = 0;
        DAT_0067ece4 = 0;
      }
      break;
    case -0xdc0000:
    case -0xd90000:
      if (DAT_0067eac4 != 0) {
        FUN_0043d2a0(0, 2);
        DAT_0067e914 = 1;
        FUN_0040e360(1);
        DAT_0067ecb4 = 4;
        FUN_00429aa0();
        FUN_004298c0();
      }
      if (DAT_0067eac5 != 0) {
        DAT_0067f1a8 = 0;
        FUN_0043d2a0(7, 0);
        if (DAT_0067ed6c == 2) {
          DAT_0067e9fc = 3;
        }
      }
      break;
    case -0xd80000:
      if (DAT_0067eac4 != 0) {
        DAT_0067ea64 = 1;
        DAT_0067ecdc = 0;
        DAT_0067ea70 = 1;
        DAT_007f1a14[1].a = -1;
        DAT_007f1a14[2].a = -1;
        DAT_007f1a14[3].a = -1;
        DAT_007f1a14[0].b = 0;
        DAT_007f1a14[1].b = -1;
        DAT_007f1a14[2].b = -1;
        DAT_007f1a14[3].b = -1;
        p = (int *)(FUN_00413f90() + DAT_0067f17c);
        n = *p++;
        ids = p;
        skip = 0;
        for (i = 0; i < n; i++) {
          if (DAT_007f1a14[0].a == i + 1) {
            skip++;
          }
          DAT_007f1a14[i + 1].a = skip + 1 + i;
          id = *p++;
          if (id == DAT_007f1a14[0].c) {
            for (j = 0; j < 6; j++) {
              if (j != DAT_007f1a14[0].c && j != ids[0] && j != ids[1] && j != ids[2]) {
                id = j;
                break;
              }
            }
          }
          DAT_007f1a14[i + 1].c = id;
          DAT_007f1a14[i + 1].b = 1;
        }
        FUN_0046dc00(0, DAT_0067ea84);
        FUN_0043d2a0(0, 2);
        DAT_0067e914 = 1;
        FUN_0040e360(1);
        DAT_0067ecb4 = 1;
      }
      break;
    case -0xce0000:
      if (DAT_0067eac4 != 0) {
        FUN_0043d2a0(7, 0);
        FUN_00422b30();
        FUN_0040b810();
      }
      if (DAT_0067eac5 != 0) {
        DAT_0067ecb4 = 0;
        FUN_0042b950();
        FUN_0043d2a0(1, 0);
        FUN_00422b30();
        FUN_0040b810();
      }
      break;
    case -0xba0000:
      if (DAT_0067eac4 != 0) {
        FUN_0040ab40();
      }
      if (DAT_0067eac5 != 0) {
        FUN_0040ac80();
      }
      break;
    case -0x800000:
      if (DAT_0067eac4 != 0) {
        PostQuitMessage(0);
      }
      break;
    }
    if (DAT_0067e930 != 0) {
      FUN_0042bf30(DAT_0067e918, DAT_0067e91c, DAT_0067e920, DAT_0067e924, DAT_0067e928, DAT_0067e92c);
      DAT_0067e930 = 0;
    }
  }
  if (DAT_0067eab0 == 1) {
    DAT_0067eab0 = 2;
  }
}
