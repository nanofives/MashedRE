// FLAGS: /O2 /GS /Ob1
/* 0x0040b180: 100% (regs 100%). Raw g2s 60.9%. Five 4-element int arrays, two constant-count loops that 9176
   fully unrolls. The second loop's statement order decides both register roles and which array gets which store
   block: of the 24 orders of the 4 statements, 4 reach 100% on masked bytes but only 2 also put every relocation
   on the original's address (checked per reloc: symbol + addend vs the original operand). This order
   (9510, 9570, 94e0, 9520) is one of those 2. No residual. */
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
MGLOBALA(int, DAT_008a94e0, 4, 0x008a94e0);
MGLOBALA(int, DAT_008a9500, 4, 0x008a9500);
MGLOBALA(int, DAT_008a9510, 4, 0x008a9510);
MGLOBALA(int, DAT_008a9520, 4, 0x008a9520);
MGLOBALA(int, DAT_008a9570, 4, 0x008a9570);

int FUN_0040e340(void);
int FUN_0042f500(void);

// MATCH: 0x0040b180 FUN_0040b180
void FUN_0040b180(void)
{
  int v;
  int i;

  v = 6;
  if (FUN_0040e340() == 2) {
    v = 4;
  }
  if (FUN_0040e340() == 3) {
    v = 4;
  }
  if (FUN_0042f500() != 0) {
    v = 4;
  }
  if ((DAT_007f0fd0 == 1) || (DAT_007f0fd0 == 2)) {
    v = 4;
  }
  for (i = 0; i < 4; i++) {
    DAT_008a9500[i] = -1000;
  }
  for (i = 0; i < 4; i++) {
    DAT_008a9510[i] = 0;
    DAT_008a9570[i] = v;
    DAT_008a94e0[i] = v;
    DAT_008a9520[i] = -1000;
  }
}
