// FLAGS: /O2 /GS /Ob1
/* RACE-PIZ-DEVICE 0x004b6c00: piz: device method slot +0x28 (open): name into the 0x40-byte ring 0x008ab860 (count DAT_008ad864, < 0x80), strcpy to 0x008ab6e0, __stricmp (0x004b6b10) over the 0x0090dac0 entries; on a hit stores the entry at +0x20064, clears the 0x20000 buffer +0x64 and +0x2006c / +0x20068 / +0x20070, 0x004b6b00(f + 8, 0), returns 1; else 3. */
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
MGLOBAL(unsigned int, DAT_008ad864, 0x008ad864);
MGLOBAL(unsigned int, DAT_008ad9a8, 0x008ad9a8);
MGLOBALA(char, DAT_008ab6e0, 0x100, 0x008ab6e0);
typedef char Ring_008ab860[0x40];
MGLOBALA(Ring_008ab860, DAT_008ab860, 0x80, 0x008ab860);
MGLOBALA(PizEnt_004b6cd0, DAT_0090dac0, 0x80, 0x0090dac0);
char *FUN_004a34b0(char *p0, const char *p1, unsigned int p2);
int FUN_004b6b10(const char *p0, const char *p1);
void *memset(void *, int, unsigned int);
char *strcpy(char *, const char *);
#pragma intrinsic(memset, strcpy)

/* 0x004b6b00 (re/match/s18/004b6b00.c, C4): static register-arg helper, ECX = destination, EAX = value */
static void FUN_004b6b00(int *param_1, int param_2)
{
  *param_1 = param_2;
}

// MATCH: 0x004b6c00 FUN_004b6c00
int FUN_004b6c00(void *param_1, PizFile_004b6cd0 *param_2, char *param_3)
{
  unsigned int i;
  PizEnt_004b6cd0 *e;

  if (DAT_008ad864 < 0x80) {
    FUN_004a34b0(DAT_008ab860[DAT_008ad864], param_3, 0x40);
    DAT_008ab860[DAT_008ad864][0x3f] = 0;
  }
  DAT_008ad864++;
  strcpy(DAT_008ab6e0, param_3);
  for (i = 0, e = DAT_0090dac0; i < DAT_008ad9a8; i++, e++) {
    if (FUN_004b6b10(e->name, param_3) == 0) {
      param_2->ent = e;
      memset(param_2->buf, 0, sizeof(param_2->buf));
      param_2->wend = 0;
      param_2->wstart = 0;
      param_2->cur = 0;
      FUN_004b6b00(&param_2->f8, 0);
      return 1;
    }
  }
  return 3;
}
