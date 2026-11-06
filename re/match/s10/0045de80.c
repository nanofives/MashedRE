// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x0045de80 (h3): 52.9% (regs 69.0%, ops 86.2%). Raw converter 19.8%. Types: 150 x 0x28-byte
   records at 0x0068d558 (16-byte name filled by an inline memset, +0x14/+0x18/+0x20/+0x1c/+0x24 zeroed in that
   order), the track table PTR_DAT_00612f30[param_2] is 40 x 0x30-byte entries {name[16], val, count, pairs[3]}.
   The memset fill value in the original is the ADDRESS of the empty string 0x005cc4de (mov ecx, 0x5cc4de; then
   the cl byte splat), so the source passed a pointer as the fill value: 9176 folds it to mov cl, imm8 under every
   spelling tried ((int)"", (unsigned)/(long)/(char*) casts, MGLOBALA char array, a const char* local).
   Remaining: register allocation (original keeps a shared zero in ebp from before the first loop, spills val to
   [esp+0x10] and keeps the pair counter in param_2's slot). Tried: index/pointer/do-while forms of all three
   loops, Ghidra-literal param_2 reuse, per-entry pointer, permuter 300 s (no gain). Residual class (b). */
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
  char name[16];
  int f10;
  int f14;
  int f18;
  int f1c;
  int f20;
  int f24;
} Z_0045de80;
typedef struct {
  int start;
  int n;
} R_0045de80;
typedef struct {
  char name[16];
  int val;
  int count;
  R_0045de80 r[3];
} T_0045de80;
MGLOBAL(T_0045de80 *, PTR_DAT_00612f30, 0x00612f30);
MGLOBALA(Z_0045de80, DAT_0068d558, 150, 0x0068d558);
MGLOBALA(char, DAT_0068ecc8, 16, 0x0068ecc8);
void *memset(void *d, int c, unsigned int n);
#pragma intrinsic(memset)
// MATCH: 0x0045de80 FUN_0045de80
void FUN_0045de80(char *param_1,int param_2)
{
  T_0045de80 *t;
  Z_0045de80 *z;
  int i;
  int j;
  int k;
  int val;

  t = (&PTR_DAT_00612f30)[param_2];
  for (i = 0; i < 150; i++) {
    memset(DAT_0068d558[i].name, (int)"", 16);
    DAT_0068d558[i].f14 = 0;
    DAT_0068d558[i].f18 = 0;
    DAT_0068d558[i].f20 = 0;
    DAT_0068d558[i].f1c = 0;
    DAT_0068d558[i].f24 = 0;
  }
  for (i = 0; i < 16; i++) {
    t->name[i] = param_1[i];
    DAT_0068ecc8[i] = param_1[i];
  }
  for (i = 0; i < 40; i++) {
    val = t[i].val;
    for (j = 0; j < t[i].count; j++) {
      z = &DAT_0068d558[t[i].r[j].start];
      for (k = t[i].r[j].n; k > 0; k--) {
        z->f20 = i + 1;
        z->f1c = val;
        z++;
      }
    }
  }
}
