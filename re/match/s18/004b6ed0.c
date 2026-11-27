// FLAGS: /O2 /GS /Ob1
/* RACE-PIZ-DEVICE 0x004b6ed0: piz: device method slot +0x38 (seek): DAT_008ad86c += 1; mode 1 cur = off, 2 cur += off, 3 cur = ent->len + off; 0x004b6b00(f + 8, cur) and 0x004b6b00(&local, cur); returns the 8-byte local in EDX:EAX (0x004b6f47 / 0x004b6f4b), its high dword never written. */
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
MGLOBAL(int, DAT_008ad86c, 0x008ad86c);

/* 0x004b6b00 (re/match/s18/004b6b00.c, C4): static register-arg helper, ECX = destination, EAX = value */
static void FUN_004b6b00(int *param_1, int param_2)
{
  *param_1 = param_2;
}

// MATCH: 0x004b6ed0 FUN_004b6ed0
__int64 FUN_004b6ed0(PizFile_004b6cd0 *param_1, int param_2, int param_3)
{
  __int64 r;

  DAT_008ad86c++;
  if (param_3 == 1) {
    param_1->cur = param_2;
  }
  else if (param_3 == 2) {
    param_1->cur += param_2;
  }
  else if (param_3 == 3) {
    param_1->cur = param_1->ent->len + param_2;
  }
  FUN_004b6b00(&param_1->f8, param_1->cur);
  FUN_004b6b00((int *)&r, param_1->cur);
  return r;
}
