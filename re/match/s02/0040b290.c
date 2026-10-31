// FLAGS: /O2 /GS /Ob1
/* 0x0040b290: 86.6% (regs 86.6%, ops 97.3%), 112/112 instructions. Raw g2s 67.9%.
   What moved it: the 0x007e9de0 event queue as ONE struct {count; type[256]; a[256]; b[256]; c[256][8]}, so every
   store through it reloads count as in the original (separate arrays let count stay in a register); the
   team-compare as nested if/else with the FUN_0042f500 != 0 arm first, so both compares share one je.
   Residual (b) register allocation: the original keeps 0 in edi (cmp eax,edi / cmp esi,edi / mov [count],edi /
   mov [94e0+p*4],edi), ours puts the CSE'd address &DAT_008a94e0[param_1] in edi and uses test/imm 0. Tried:
   switch / nested-if / inverted-if forms of the FUN_0040e350 test, += vs a = a + b, 6000 store first, uint
   param_2, a struct over 0x008a94e0..0x008a957c, an __inline queue-push helper (not inlined, 75%), and the
   permuter (68 variants, no gain). Budget spent. */
/* ---- MATCH-S02 preamble (same text in every re/match/s02 source; not part of any function body) ----
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
typedef struct { int team; int a, b, c; } Ent18;
typedef struct { uint count; int type[256]; int a[256]; int b[256]; int c[256][8]; } EvQ;
MGLOBAL(EvQ, DAT_007e9de0, 0x007e9de0);
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);
MGLOBAL(int, DAT_007f0fd4, 0x007f0fd4);
MGLOBAL(int, DAT_007f1014, 0x007f1014);
MGLOBAL(int, DAT_007f1030, 0x007f1030);
MGLOBALA(Ent18, DAT_007f1a18, 4, 0x007f1a18);
MGLOBALA(int, DAT_008a94e0, 4, 0x008a94e0);
MGLOBALA(int, DAT_008a9500, 4, 0x008a9500);
MGLOBALA(int, DAT_008a9510, 4, 0x008a9510);
MGLOBALA(int, DAT_008a9520, 4, 0x008a9520);
MGLOBALA(int, DAT_008a9570, 4, 0x008a9570);

int FUN_0040e340(void);
int FUN_0040e350(void);
int FUN_0040e370(int param_1);
int FUN_0042f500(void);

// MATCH: 0x0040b290 FUN_0040b290
void FUN_0040b290(int param_1, int param_2)
{
  int n;
  int i;
  int m;

  if (DAT_007f0fd0 == 1) {
    if (param_2 < 0) {
      param_2 = 0;
    }
    n = 0;
    for (i = 0; i < 4; i++) {
      if (FUN_0040e370(i) != 0) {
        n++;
      }
    }
    if (n == 4 && FUN_0042f500() == 0) {
      param_2 = (param_2 == 2);
    }
    if (FUN_0042f500() != 0) {
      if (DAT_007f1a18[DAT_007f0fd4].team != DAT_007f1a18[param_1].team) {
        param_2 = 0;
      }
    } else if (param_1 != DAT_007f0fd4) {
      param_2 = 0;
    }
  }
  if (DAT_007f0fd0 == 2) {
    if (param_2 < 0) {
      param_2 = 0;
    }
    if (DAT_007f1a18[param_1].team == 1) {
      param_2 = 0;
    }
    if (FUN_0040e340() == 4) {
      param_2 = (param_2 == 2);
    } else if (param_2 == 2) {
      param_2 = 1;
    }
  }
  DAT_008a9570[param_1] = DAT_008a94e0[param_1];
  DAT_008a9520[param_1] = param_2;
  if (DAT_007f1014 != 0 && param_2 < 0) {
    param_2 = 0;
  }
  DAT_008a9500[param_1] = param_2;
  m = FUN_0040e350();
  if (m != 1 && m != 2 && m != 3 && m != 4 && m != 5) {
    DAT_007e9de0.type[DAT_007e9de0.count] = 0x2ff01;
    DAT_007e9de0.a[DAT_007e9de0.count] = DAT_007f1030;
    DAT_007e9de0.b[DAT_007e9de0.count] = param_1;
    DAT_007e9de0.c[DAT_007e9de0.count][0] = param_2;
    DAT_007e9de0.count++;
    if (DAT_007e9de0.count >= 256) {
      DAT_007e9de0.count = 0;
    }
  }
  DAT_008a94e0[param_1] += param_2;
  DAT_008a9510[param_1] = 6000;
  if (DAT_008a94e0[param_1] < 0) {
    DAT_008a94e0[param_1] = 0;
  }
}
