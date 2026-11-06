// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 h5 0x004989b0: 100% (regs 100, ops 100). videocfg.bin name/load/save TU: FUN_00498910 (inlined strcpy
   of the literal "videocfg.bin", the compiler's copy of the 0x005d012c string), FUN_00498950 (fopen rb + fread
   0x200 into 0x00773208), FUN_004989b0 (fopen wb + fwrite). Same three functions in each of the three files. */
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
typedef struct FILE_ FILE;
MGLOBALA(char, DAT_007731e8, 16, 0x007731e8);
MGLOBALA(char, DAT_00773208, 0x200, 0x00773208);
MGLOBALA(char, s_Loading_005d013c, 27, 0x005d013c);
MGLOBALA(char, s_Saving_005d0158, 24, 0x005d0158);
MGLOBALA(char, DAT_005cf010, 3, 0x005cf010);
MGLOBALA(char, DAT_005d00d8, 3, 0x005d00d8);
MGLOBALA(char, s_FAILED_005cfe70, 9, 0x005cfe70);
char *strcpy(char *, const char *);
#pragma intrinsic(strcpy)
void FUN_004963e0(const char *fmt, ...);
void FUN_00496400(const char *fmt, ...);
FILE *FUN_004a4541(const char *name, const char *mode);
unsigned int FUN_004a49cf(void *buf, unsigned int size, unsigned int n, FILE *f);
unsigned int FUN_004a4b22(const void *buf, unsigned int size, unsigned int n, FILE *f);
int FUN_004a4368(FILE *f);

// MATCH: 0x00498910 FUN_00498910
char *FUN_00498910(void)
{
  strcpy(DAT_007731e8, "videocfg.bin");
  return DAT_007731e8;
}

// MATCH: 0x00498950 FUN_00498950
int FUN_00498950(void)
{
  char *name = FUN_00498910();
  FILE *f;
  FUN_00496400(s_Loading_005d013c, name);
  f = FUN_004a4541(name, DAT_005cf010);
  if (f) {
    FUN_004a49cf(DAT_00773208, 1, 0x200, f);
    FUN_004a4368(f);
    return 1;
  }
  FUN_004963e0(s_FAILED_005cfe70);
  return 0;
}

// MATCH: 0x004989b0 FUN_004989b0
int FUN_004989b0(void)
{
  char *name = FUN_00498910();
  FILE *f;
  FUN_00496400(s_Saving_005d0158, name);
  f = FUN_004a4541(name, DAT_005d00d8);
  if (f) {
    FUN_004a4b22(DAT_00773208, 1, 0x200, f);
    FUN_004a4368(f);
    return 1;
  }
  return 0;
}
