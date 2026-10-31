// FLAGS: /O2 /GS /Ob1
/* 0x0040b700 (129 B; 148 B symbol with the jump table): 100.0% exact, regs 100.0%, whole obj symbol
   BYTE-IDENTICAL. Raw g2s did not compile (Ghidra's cross-jumped gotos). Source form: mode =
   FUN_0040e340() unless FUN_0042f500() != 0 and 0x007f0fd0 != 2 (then 2), outer switch on mode
   {2, 3, 4} with an inner switch on param_2 each, incrementing one of the int[4] counters at
   0x008a9530/40/50/60. The shared ret-blocks are the compiler's tail merging. What moved the last
   3 bytes: in mode 4, case 1 written before case 0. Residual: none. */
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
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);
MGLOBALA(int, DAT_008a9530, 4, 0x008a9530);
MGLOBALA(int, DAT_008a9540, 4, 0x008a9540);
MGLOBALA(int, DAT_008a9550, 4, 0x008a9550);
MGLOBALA(int, DAT_008a9560, 4, 0x008a9560);

int FUN_0040e340(void);
int FUN_0042f500(void);

// MATCH: 0x0040b700 FUN_0040b700
void FUN_0040b700(int param_1, int param_2)
{
  int mode;

  if (FUN_0042f500() == 0 || DAT_007f0fd0 == 2) {
    mode = FUN_0040e340();
  } else {
    mode = 2;
  }
  switch (mode) {
  case 2:
    switch (param_2) {
    case 0:
      DAT_008a9560[param_1]++;
      break;
    case 1:
      DAT_008a9530[param_1]++;
      break;
    }
    break;
  case 3:
    switch (param_2) {
    case 0:
      DAT_008a9540[param_1]++;
      break;
    case 1:
      DAT_008a9560[param_1]++;
      break;
    case 2:
      DAT_008a9530[param_1]++;
      break;
    }
    break;
  case 4:
    switch (param_2) {
    case 1:
      DAT_008a9540[param_1]++;
      break;
    case 0:
      DAT_008a9550[param_1]++;
      break;
    case 2:
      DAT_008a9560[param_1]++;
      break;
    case 3:
      DAT_008a9530[param_1]++;
      break;
    }
    break;
  }
}
