// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x004648b0 (h8): 100% by hand. What moved it: the value at 0x007ec9e8 (stride 64) and the 4th
   parameter of FUN_00464670 are float, so the hoisted copy lives in a stack slot and the inner switch keeps its
   identical case 0 / case 3 bodies apart (as int they merge, 58-63%). Case 0xb is a switch on the (int) of the
   float at 0x007ec9e4 with two separate identical bodies (sub eax,0 / dec eax form); inner cases in the order
   1, 0, 3, 4 (code order of the original). */
/* ---- MATCH-S10 preamble (same text in every re/match/s10 source; not part of any function body) ----
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
MGLOBAL(int, DAT_007e9de0, 0x007e9de0);
MGLOBAL(int, DAT_007f1030, 0x007f1030);
MGLOBALA(int, DAT_007e9de4, 256, 0x007e9de4);
MGLOBALA(int, DAT_007ea1e4, 256, 0x007ea1e4);
MGLOBALA(int, DAT_007ea5e4, 256, 0x007ea5e4);
MGLOBALA(int, DAT_007ea9e8, 2048, 0x007ea9e8);
MGLOBALA(int, DAT_007ea9ec, 2048, 0x007ea9ec);
MGLOBALA(float, DAT_007ec9e4, 4096, 0x007ec9e4);
MGLOBALA(float, DAT_007ec9e8, 4096, 0x007ec9e8);
void FUN_00464670(int a, int b, int c, float d);
// MATCH: 0x004648b0 FUN_004648b0
void FUN_004648b0(void)
{
  int key;
  int idx;
  int n;
  int done;
  int id;
  float v;

  idx = DAT_007e9de0 - 1;
  if (idx < 0) {
    idx = 0xff;
  }
  key = DAT_007f1030;
  n = 0x100;
  done = 0;
  do {
    if (done) {
      return;
    }
    if (DAT_007ea1e4[idx] == key) {
      id = DAT_007ea5e4[idx] - 0xff00;
      switch (DAT_007e9de4[idx]) {
      case 3:
        FUN_00464670(id, 0, 0, 0);
        break;
      case 4:
        FUN_00464670(id, 1, DAT_007ea9e8[idx * 8], 0);
        break;
      case 8:
        v = DAT_007ec9e8[idx * 16];
        switch (DAT_007ea9ec[idx * 8]) {
        case 1:
          FUN_00464670(id, 2, 0, v);
          break;
        case 0:
          FUN_00464670(id, 3, 0, v);
          break;
        case 3:
          FUN_00464670(id, 3, 0, v);
          break;
        case 4:
          FUN_00464670(id, 4, 0, v);
          break;
        }
        break;
      case 0xb:
        switch ((int)DAT_007ec9e4[idx * 16]) {
        case 0:
          FUN_00464670(id, 3, 0, 0);
          break;
        case 1:
          FUN_00464670(id, 3, 0, 0);
          break;
        }
        break;
      }
    }
    else {
      done = 1;
    }
    idx--;
    if (idx < 0) {
      idx = 0xff;
    }
    n--;
  } while (0 < n);
}
