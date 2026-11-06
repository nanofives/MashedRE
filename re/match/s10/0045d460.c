// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x0045d460 (h3): 94.2% (regs 97.7%, ops 98.8%). Raw converter 60.2% (no /GS cookie). Fixed: dead
   char buf[8] for the /GS cookie (8-byte frame = cookie + spilled outer counter), 0x94-byte voice-bank record at
   0x006044f0 {name[0x80], first, count, flag, v8c, v90}, RwGlobals (DAT_007d3ff8) slots +0x114/+0x10c as function
   pointers, FUN_005a7af0 takes one argument, FUN_005a6dc0 four, flag test as if (flag == 0) &= ~4 else |= 4 (the
   compiler tail-merges the two FUN_005a6dc0(...,8,1,x) calls), strings as literals. Remaining: only the
   voice-count sum loop (0x0045d5c4..0x0045d5e8): the original accumulates in edx and stores that register (not
   the ebx zero) to DAT_0068fc84 before the loop, pointer in eax / count in ecx. Tried: global += form, local
   accumulator with the store inside the if / after the if / after the loop, every loop-variable choice, pointer
   walk, count local, permuter 300 s (its 94.1% moves FUN_005a71f0 below the store: not adopted). Residual
   class (b). */
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
typedef struct {
  char name[0x80];
  unsigned int first;
  unsigned int count;
  int flag;
  int v8c;
  int v90;
} W_0045d460;
typedef int (*Fn3_0045d460)(int, int, int);
typedef void (*Fn1_0045d460)(int);
MGLOBAL(char *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007dcaa8, 0x007dcaa8);
MGLOBAL(int, DAT_007dcaac, 0x007dcaac);
MGLOBAL(int, DAT_0069047c, 0x0069047c);
MGLOBAL(int, DAT_0069049c, 0x0069049c);
MGLOBAL(unsigned int, DAT_0068fc88, 0x0068fc88);
MGLOBAL(unsigned int, DAT_006904a0, 0x006904a0);
MGLOBAL(unsigned int, DAT_0068fc84, 0x0068fc84);
MGLOBAL(unsigned int, DAT_0088e674, 0x0088e674);
MGLOBAL(W_0045d460, DAT_006044f0, 0x006044f0);
MGLOBAL(int, DAT_0068f560, 0x0068f560);
MGLOBAL(int, DAT_0068f640, 0x0068f640);
MGLOBALA(int, DAT_0068fcd0, 1, 0x0068fcd0);
int FUN_004a2cbd(const char *fmt, ...);
int FUN_004cc160(int p0, int p1);
int FUN_004cc230(int p0, int p1, const char *p2);
int FUN_004cc5e0(int p0, int p1, int p2, int p3);
int FUN_005a6280(int p0, int p1, int p2, int p3, int p4);
void FUN_005a6710(int p0, int p1);
void FUN_005a6dc0(int p0, int p1, int p2, int p3);
int FUN_005a71f0(int p0, int *p1, unsigned int p2);
int FUN_005a7aa0(int p0, char *p1);
unsigned int FUN_005a7af0(int p0);
int FUN_005a7b60(int p0, int p1, int p2, int p3, int p4, int p5);
int FUN_005b8570(int p0, int p1, int p2, int p3, int p4);
// MATCH: 0x0045d460 FUN_0045d460
void FUN_0045d460(void)
{
  char buf[8]; /* unused: only makes /GS emit the cookie */
  int h;
  int p;
  unsigned int i;
  unsigned int j;
  unsigned int m;
  unsigned int k;

  h = FUN_004cc230(2,1,"toastaudio\pc\audio\pcdics\PERMDICT.RWS");
  p = ((Fn3_0045d460)*(void **)(DAT_007d3ff8 + 0x114))(0x10001,1,0);
  if (p != 0) {
    DAT_007dcaa8 = p;
    DAT_007dcaac = 0x10000;
  }
  else {
    DAT_007dcaa8 = 0;
    DAT_007dcaac = 0;
  }
  DAT_0069047c = 0;
  while (FUN_004cc5e0(h,0x809,0,0) != 0) {
    DAT_0069047c = FUN_005a7b60(0,h,DAT_0069049c,0,0,0);
  }
  ((Fn1_0045d460)*(void **)(DAT_007d3ff8 + 0x10c))(p);
  FUN_004cc160(h,0);
  DAT_0068fc88 = FUN_005a7af0(DAT_0069047c);
  FUN_004a2cbd("numpermwaves %d\n",DAT_0068fc88);
  for (i = 0; i < DAT_0068fc88; i++) {
    (&DAT_0068f560)[i] = FUN_005a7aa0(DAT_0069047c,(&DAT_006044f0)[i].name);
  }
  for (i = 0; i < DAT_006904a0; i++) {
    DAT_0068fcd0[i] = FUN_005b8570(DAT_0069049c,0,2,0,0);
  }
  FUN_005a71f0(DAT_0069049c,DAT_0068fcd0,DAT_006904a0);
  k = 0;
  DAT_0068fc84 = 0;
  for (i = 0; i < DAT_0068fc88; i++) {
    if ((&DAT_006044f0)[i].count > 0) {
      k += (&DAT_006044f0)[i].count;
    }
    DAT_0068fc84 = k;
  }
  for (i = 0; i < 200; i++) {
    (&DAT_0068f640)[i * 2] = FUN_005a6280(0,0,0,0,0);
  }
  for (i = 0; i < DAT_0068fc84; i++) {
    *(unsigned int *)((&DAT_0068f640)[i * 2] + 0x34) |= 4;
    if (*(int *)((&DAT_0068f640)[i * 2] + 0x30) != 0) {
      FUN_005a6dc0(*(int *)((&DAT_0068f640)[i * 2] + 0x30),8,1,1);
    }
    *(int *)((&DAT_0068f640)[i * 2] + 0x44) = 0;
    if (*(int *)((&DAT_0068f640)[i * 2] + 0x30) != 0) {
      FUN_005a6dc0(*(int *)((&DAT_0068f640)[i * 2] + 0x30),10,1,0);
    }
  }
  k = 0;
  for (j = 0; j < DAT_0068fc88; j++) {
    (&DAT_006044f0)[j].first = k;
    for (m = 0; m < (&DAT_006044f0)[j].count; m++) {
      FUN_005a6710((&DAT_0068f640)[k * 2],(&DAT_0068f560)[j]);
      if ((&DAT_006044f0)[j].flag == 0) {
        *(unsigned int *)((&DAT_0068f640)[k * 2] + 0x34) &= ~4u;
        if (*(int *)((&DAT_0068f640)[k * 2] + 0x30) != 0) {
          FUN_005a6dc0(*(int *)((&DAT_0068f640)[k * 2] + 0x30),8,1,0);
        }
      }
      else {
        *(unsigned int *)((&DAT_0068f640)[k * 2] + 0x34) |= 4;
        if (*(int *)((&DAT_0068f640)[k * 2] + 0x30) != 0) {
          FUN_005a6dc0(*(int *)((&DAT_0068f640)[k * 2] + 0x30),8,1,1);
        }
      }
      *(int *)((&DAT_0068f640)[k * 2] + 0x74) = (&DAT_006044f0)[j].v8c;
      if (*(int *)((&DAT_0068f640)[k * 2] + 0x30) != 0) {
        FUN_005a6dc0(*(int *)((&DAT_0068f640)[k * 2] + 0x30),3,1,(&DAT_006044f0)[j].v8c);
      }
      k++;
    }
  }
  DAT_0068fc84 = k;
  DAT_0088e674 = k;
  FUN_004a2cbd("numpermvirtualvoices %d\n",k);
}
