// FLAGS: /O2 /GS /Ob1
/* RACE-PIZ-DEVICE 0x004b6f60: piz: device method slot +0x2c (close): DAT_008ad870 += 1, clears +0x20064 / +0x20068 / +0x2006c / +0x20070, tail-jumps to 0x004b6b00 (ECX = f + 8, EAX = 0) at 0x004b6f89. */
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
MGLOBAL(int, DAT_008ad870, 0x008ad870);

/* 0x004b6b00 (re/match/s18/004b6b00.c, C4): static register-arg helper, ECX = destination, EAX = value */
static void FUN_004b6b00(int *param_1, int param_2)
{
  *param_1 = param_2;
}

// MATCH: 0x004b6f60 FUN_004b6f60
void FUN_004b6f60(PizFile_004b6cd0 *param_1)
{
  DAT_008ad870++;
  param_1->ent = 0;
  param_1->wstart = 0;
  param_1->wend = 0;
  param_1->cur = 0;
  FUN_004b6b00(&param_1->f8, 0);
}
