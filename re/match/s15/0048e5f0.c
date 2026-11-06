// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 h2 0x0048e5f0: 100% (regs 100%), whole symbol byte-identical. Hand rewrite (raw 33%). What moved it: pointer walk p over the 8 slot words with j and p initialised in the for header (67% -> 98%: it also fixed the register rotation of the tail-merged case bodies), switch cases in source order 15,16,14,7,9,10,8,13,1,6 each calling FUN_00476df0 itself, i = 17 before the FUN_00467210 call, and DAT_007668f4 file-static under C (pilot finding 2: the second fld [ecx+4] is scheduled before the store of 1 only when the global cannot alias). Under C++ it stays MGLOBAL. Residual: none. */
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
MGLOBAL(float, DAT_005cd30c, 0x005cd30c);
MGLOBAL(float, DAT_005ccac0, 0x005ccac0);
#ifndef __cplusplus
static int DAT_007668f4;
#else
MGLOBAL(int, DAT_007668f4, 0x007668f4);
#endif
MGLOBALA(int, DAT_00722250, 17 * 8, 0x00722250);
MGLOBALA(unsigned char, DAT_00722470, 17 * 0x40, 0x00722470);
MGLOBALA(unsigned char, DAT_007228b0, 17 * 0x4000, 0x007228b0);
char *FUN_00467210(int p0);
void FUN_00476df0(void *p0);
void FUN_0048d140(void *p0);
void FUN_0048d360(void *p0);
void FUN_0048d540(void *p0);
void FUN_0048d720(void *p0);
void FUN_0048d900(void *p0);
void FUN_0048db20(void *p0);
void FUN_0048dd40(void *p0);
void FUN_0048df60(void *p0);
void FUN_0048e180(void *p0);
void FUN_0048e360(void *p0);
void FUN_004b6480(void *p0, unsigned int p1);
// MATCH: 0x0048e5f0 FUN_0048e5f0
void FUN_0048e5f0(void)
{
  float *v;
  int i;
  int j;
  int *p;
  void *buf;

  i = 17;
  v = (float *)(FUN_00467210(-1) + 0x20);
  if (v[1] > DAT_005cd30c) {
    DAT_007668f4 = 1;
    if (v[1] < DAT_005ccac0) {
      goto skip;
    }
  }
  DAT_007668f4 = 0;
skip:
  do {
    i--;
    for (j = 0, p = &DAT_00722250[i * 8]; j < 8; j++, p++) {
      if (*p != 0) {
        buf = &DAT_007228b0[i * 0x4000];
        switch (i) {
        case 15:
          FUN_0048db20(buf);
          FUN_00476df0(&DAT_00722470[i * 0x40]);
          break;
        case 16:
          FUN_0048dd40(buf);
          FUN_00476df0(&DAT_00722470[i * 0x40]);
          break;
        case 14:
          FUN_0048e360(buf);
          FUN_00476df0(&DAT_00722470[i * 0x40]);
          break;
        case 7:
          FUN_0048d140(buf);
          FUN_00476df0(&DAT_00722470[i * 0x40]);
          break;
        case 9:
          FUN_0048d360(buf);
          FUN_00476df0(&DAT_00722470[i * 0x40]);
          break;
        case 10:
          FUN_0048d540(buf);
          FUN_00476df0(&DAT_00722470[i * 0x40]);
          break;
        case 8:
          FUN_0048d720(buf);
          FUN_00476df0(&DAT_00722470[i * 0x40]);
          break;
        case 13:
          FUN_0048d900(buf);
          FUN_00476df0(&DAT_00722470[i * 0x40]);
          break;
        case 1:
          FUN_0048df60(buf);
          FUN_00476df0(&DAT_00722470[i * 0x40]);
          break;
        case 6:
          FUN_0048e180(buf);
          FUN_00476df0(&DAT_00722470[i * 0x40]);
          break;
        default:
          FUN_004b6480(&DAT_00722250[i * 8], 0x100);
          break;
        }
        break;
      }
    }
  } while (i != 0);
}
