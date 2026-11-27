// FLAGS: /O2 /GS /Ob1
/* RACE-PIZ-DEVICE 0x004b6ba0: piz: device method slot +0x4c (exists): DAT_008ad860 += 1, __stricmp (0x004b6b10) of the name against each 0x80-byte entry of 0x0090dac0, count DAT_008ad9a8; 1 on a hit, else 0. */
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
typedef struct { char name[0x74]; unsigned int base; unsigned int len; int pad7c; } PizEnt_004b6cd0;
typedef struct {
  int f0;
  int f4;
  int f8;
  char pad0c[0x64 - 0xc];
  unsigned char buf[0x20000];
  PizEnt_004b6cd0 *ent;
  unsigned int wstart;
  unsigned int wend;
  unsigned int cur;
  int f20074;
} PizFile_004b6cd0;
MGLOBAL(int, DAT_008ad860, 0x008ad860);
MGLOBAL(unsigned int, DAT_008ad9a8, 0x008ad9a8);
MGLOBALA(PizEnt_004b6cd0, DAT_0090dac0, 0x80, 0x0090dac0);
int FUN_004b6b10(const char *p0, const char *p1);

// MATCH: 0x004b6ba0 FUN_004b6ba0
int FUN_004b6ba0(void *param_1, char *param_2)
{
  unsigned int i;

  DAT_008ad860++;
  for (i = 0; i < DAT_008ad9a8; i++) {
    if (FUN_004b6b10(DAT_0090dac0[i].name, param_2) == 0) {
      return 1;
    }
  }
  return 0;
}
