// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x00466b50: 100% (regs 100%, whole symbol byte-identical incl. both jump tables), by hand (raw g2s
   did not compile). What moved it: switch case order 0,1,2,3/7,5,4,6 (the original's body layout); the four
   zero stores at 0x006904a4 are one inlined memset (#pragma intrinsic) of a 4-int array, which is where the
   separate xor ecx,ecx zero comes from next to the ebx zero register; the 1..4 test of FUN_0042b900 in case 1 is
   a switch (case 1-3 / case 4), and in case 4 a switch with separate case 2 and case 3 bodies (sub/dec chain);
   FUN_00426c00's result in a local before == FUN_00462520() (cmp esi,eax order); st != 3 tested first in the
   case 5 loop; float globals (0x006900d4, 0x006036c4, 0x006036c0, 0x0088e678) and FUN_0045df70(0.0f).
   0x0045b350 is a one-byte ret that the linker folded several empty functions into: the two branches on
   FUN_00492d10() != 3 call it through two different declarations (the else through a __stdcall cast), so the
   compiler does not merge the two calls; the real source called two distinct empty functions. */
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
MGLOBAL(int, DAT_006904e8, 0x006904e8);
MGLOBAL(int, DAT_008aa254, 0x008aa254);
MGLOBAL(int, DAT_008aa310, 0x008aa310);
MGLOBAL(float, DAT_006900d4, 0x006900d4);
MGLOBAL(float, DAT_006036c4, 0x006036c4);
MGLOBAL(int, DAT_006904f8, 0x006904f8);
MGLOBAL(int, DAT_006904fc, 0x006904fc);
MGLOBAL(int, DAT_006904f4, 0x006904f4);
MGLOBAL(int, DAT_006904e0, 0x006904e0);
MGLOBAL(int, DAT_006904e4, 0x006904e4);
MGLOBALA(int, DAT_00690510, 4, 0x00690510);
MGLOBALA(int, DAT_0069045c, 4, 0x0069045c);
MGLOBAL(float, DAT_006036c0, 0x006036c0);
MGLOBALA(int, DAT_006904a4, 4, 0x006904a4);
MGLOBAL(float, DAT_0088e678, 0x0088e678);
#ifdef __cplusplus
extern "C"
#endif
void *memset(void *p0, int p1, unsigned int p2);
#pragma intrinsic(memset)
int FUN_0040e340(void);
int FUN_0040e370(int p0);
int FUN_00426c00(void);
int FUN_0042b8f0(void);
int FUN_0042b900(void);
int FUN_0042f6a0(void);
float FUN_00431b50(void);
float FUN_00431b60(void);
int FUN_00432290(void);
void FUN_0045b350(void);
void FUN_0045c480(void);
void FUN_0045d3a0(void);
void FUN_0045d7a0(void);
void FUN_0045db50(int p0);
void FUN_0045dbe0(int p0);
int FUN_0045dc80(void);
void FUN_0045dd60(void);
void FUN_0045df70(float p0);
void FUN_0045dfc0(void);
void FUN_0045e460(void);
void FUN_0045e990(void);
void FUN_0045efe0(void);
void FUN_0045f5f0(void);
void FUN_0045faa0(void);
void FUN_0045ff50(void);
void FUN_00460270(void);
void FUN_00460350(void);
void FUN_00460df0(void);
void FUN_00461650(void);
void FUN_004623e0(void);
int FUN_00462520(void);
int FUN_004627b0(void);
void FUN_00462dd0(void);
void FUN_00462ec0(void);
void FUN_00462f50(void);
void FUN_004631f0(void);
void FUN_00463590(void);
void FUN_00463640(int p0);
void FUN_00463c80(int p0, int p1);
void FUN_00463f40(int p0);
void FUN_004642f0(int p0, int p1);
void FUN_004644a0(int p0);
void FUN_004645c0(int p0);
void FUN_004647f0(void);
void FUN_004648b0(void);
void FUN_00464a50(void);
void FUN_00464ca0(void);
void FUN_00464e10(void);
void FUN_004652b0(void);
void FUN_004661f0(void);
int FUN_00492d10(void);
int FUN_004d8560(int p0);
int FUN_005a89a0(int p0);
int FUN_005a89b0(int p0);
int FUN_005a89c0(int p0);
// MATCH: 0x00466b50 FUN_00466b50
void FUN_00466b50(void)
{
  int i;
  int n;
  int mode;
  int st;
  int ok;
  int a[4];
  int b[4];
  int c[4];

  if (DAT_006904e8 == 0) {
    return;
  }
  DAT_008aa254 = FUN_0040e340();
  DAT_008aa310 = FUN_00426c00();
  DAT_006900d4 = FUN_00431b50();
  if (FUN_00492d10() != 3) {
    FUN_0045b350();
  }
  else {
    ((void (__stdcall *)(void))FUN_0045b350)();
  }
  FUN_0045d7a0();
  switch (FUN_00492d10()) {
  case 0:
    FUN_0045dbe0(2);
    FUN_0045db50(1);
    break;
  case 1:
    if (FUN_00432290() != 0) {
      DAT_006900d4 = 0.0f;
    }
    FUN_0045dc80();
    if (DAT_006904f8 == 1) {
      FUN_00462ec0();
      DAT_006904f4 = 0;
    }
    FUN_0045c480();
    DAT_006904e0 = 0;
    DAT_006904e4 = 0;
    n = FUN_0042b900();
    switch (n) {
    case 1:
    case 2:
    case 3:
      FUN_0045dbe0(1);
      FUN_0045db50(0);
      break;
    case 4:
      st = FUN_00426c00();
      if (st == FUN_00462520()) {
        FUN_0045db50(0);
      }
      else {
        FUN_0045db50(1);
      }
      FUN_0045dbe0(0);
      break;
    }
    FUN_0045df70(0.0f);
    DAT_006904f8 = 0;
    DAT_006904fc = 1;
    break;
  case 2:
    if (FUN_0042f6a0() != 0xb) {
      if (DAT_006904fc == 1) {
        FUN_00462ec0();
      }
      DAT_006904fc = 0;
    }
    DAT_006904f4 = 0;
    FUN_0045dbe0(0);
    if (DAT_006904fc == 0) {
      FUN_0045db50(1);
    }
    DAT_006036c0 = 0.0f;
    break;
  case 3:
  case 7:
    FUN_0045dfc0();
    DAT_006036c4 = FUN_00431b60();
    if (FUN_00432290() != 0) {
      DAT_006036c4 = 0.0f;
    }
    FUN_00462dd0();
    FUN_0042f6a0();
    mode = FUN_00426c00();
    FUN_004648b0();
    for (i = 0; i < 4; i++) {
      if (FUN_0040e370(i) != 0) {
        FUN_00463640(i);
        FUN_004645c0(i);
        FUN_004644a0(i);
        if (FUN_004d8560(mode) != 0) {
          FUN_00463c80(i,mode);
          FUN_00463f40(i);
          FUN_004642f0(i,mode);
        }
      }
    }
    FUN_00464a50();
    switch (mode) {
    case 0:
      FUN_00464e10();
      break;
    case 2:
      FUN_00464ca0();
      FUN_0045e460();
      break;
    case 3:
      FUN_00460270();
      break;
    case 0xb:
      FUN_0045f5f0();
      break;
    case 0x19:
      FUN_0045e990();
      break;
    case 0x1a:
      FUN_00460df0();
      break;
    case 0x1e:
      FUN_0045efe0();
      break;
    case 0x21:
      FUN_004661f0();
      break;
    case 0x22:
      FUN_0045ff50();
      break;
    case 0x24:
      FUN_004652b0();
      break;
    case 0x25:
      FUN_00460350();
      break;
    case 0x26:
      FUN_00461650();
      break;
    case 0x27:
      FUN_0045faa0();
      break;
    }
    FUN_00463590();
    FUN_00462f50();
    if (FUN_004627b0() == 0 && FUN_0042f6a0() != 0xb) {
      FUN_0045d3a0();
    }
    FUN_0045db50(3);
    DAT_006904f8 = 1;
    DAT_00690510[0] = 0;
    DAT_00690510[1] = 0;
    DAT_00690510[2] = 0;
    DAT_00690510[3] = 0;
    break;
  case 5:
    memset(DAT_006904a4, 0, sizeof(DAT_006904a4));
    FUN_0045dc80();
    for (i = 0; i < 4; i++) {
      if (DAT_0069045c[i] != 0 && DAT_006904f8 != 0) {
        st = FUN_005a89a0(DAT_0069045c[i]);
        a[i] = st;
        b[i] = FUN_005a89b0(DAT_0069045c[i]);
        c[i] = FUN_005a89c0(DAT_0069045c[i]);
        if (st != 3) {
          DAT_00690510[i] = 0;
        }
        else {
          DAT_00690510[i]++;
        }
      }
    }
    if (DAT_00690510[0] > 2 && DAT_00690510[1] > 2 && DAT_00690510[2] > 2 && DAT_00690510[3] > 2) {
      ok = 1;
    }
    else {
      ok = 0;
    }
    if (DAT_006904f8 == 1 && ok) {
      FUN_00462ec0();
      DAT_006904f8 = 0;
      DAT_006904fc = 0;
      DAT_006904f4 = 0;
      DAT_0088e678 = 0.0f;
      FUN_0045c480();
    }
    FUN_0045dbe0(2);
    FUN_0045df70(0.0f);
    FUN_0045db50(0);
    break;
  case 4:
    if (FUN_00432290() != 0) {
      DAT_006900d4 = 0.0f;
    }
    n = FUN_0042b900();
    DAT_006904e0 = 0;
    switch (n) {
    case 2:
      FUN_0045dbe0(1);
      break;
    case 3:
      FUN_0045dbe0(1);
      break;
    case 4:
      FUN_0045dbe0(0);
      break;
    }
    FUN_0045df70(0.0f);
    FUN_0045db50(0);
    break;
  case 6:
    FUN_0045dbe0(2);
    FUN_0042b8f0();
    DAT_006036c0 = 0.0f;
    break;
  }
  FUN_004647f0();
  FUN_004631f0();
  FUN_004623e0();
  FUN_0045dd60();
}
