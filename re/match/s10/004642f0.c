// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x004642f0 (h7, by hand): 100%. DAT_0068f640 is an 8-byte record array (object pointer + dword);
   DAT_0088e670 points at 0x90-byte records (dword at +0x80). Float constants are source literals (0x005ce18c 0.02,
   0x005d757c 0.0, 0x005ce6b8 22050.0). What moved it: k/idx/u live in an inner block, so the spill of `r`
   (FUN_0046c730's result, 0x0046436b) reuses idx's slot (frame 0xc, not 0x10); switch with cases 0/1/2. */
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
typedef struct Obj004642f0 {
  char pad0[0x30];
  int h30;
  unsigned char f34;
  char pad35[7];
  float f3c;
} Obj004642f0;
typedef struct { Obj004642f0 *p; int q; } Rec0068f640;
typedef struct { char pad0[0x80]; int base; char pad84[0xc]; } Rec0088e670;
MGLOBAL(int, DAT_0068f4c8, 0x0068f4c8);
MGLOBAL(int, DAT_008aa254, 0x008aa254);
MGLOBAL(Rec0088e670 *, DAT_0088e670, 0x0088e670);
MGLOBALA(Rec0068f640, DAT_0068f640, 1, 0x0068f640);
MGLOBALA(float, DAT_006904a4, 1, 0x006904a4);
MGLOBALA(int, DAT_00690560, 1, 0x00690560);
int FUN_0040e350(void);
void FUN_0045daf0(int p0, int p1);
void FUN_0045e0f0(int p0, float p1);
int FUN_00461e90(int p0, int p1, int *p2, int *p3);
int FUN_0046c730(int p0);
int FUN_0046d2e0(int p0, int p1, int *p2);
float FUN_00472650(float lo, float hi);
void FUN_005a66d0(Obj004642f0 *p0, int p1);
void FUN_005a6dc0(int p0, int p1, int p2, float p3);

// MATCH: 0x004642f0 FUN_004642f0
void FUN_004642f0(int param_1, int param_2)
{
  int i;
  int r;
  Obj004642f0 *o;

  if (DAT_0068f4c8 == 0 || DAT_008aa254 <= 1) {
    {
      int k;
      int idx;
      int u;
      FUN_0046d2e0(param_1, 2, &k);
      FUN_00461e90(param_2, k, &idx, &u);
      i = DAT_0088e670[idx].base + param_1;
    }
    r = FUN_0046c730(param_1);
    switch (DAT_00690560[param_1]) {
    case 0:
      if (r != 0) {
        DAT_00690560[param_1] = 1;
      }
      return;
    case 1:
      o = DAT_0068f640[i].p;
      DAT_006904a4[param_1] = 1.0f;
      if ((o->f34 & 2) == 0) {
        FUN_005a66d0(o, 1);
      }
      DAT_0068f640[i].p->f3c = 22050.0f - FUN_00472650(0.0f, 441.0f);
      if (DAT_0068f640[i].p->h30 != 0) {
        FUN_005a6dc0(DAT_0068f640[i].p->h30, 4, 1, 22050.0f - FUN_00472650(0.0f, 441.0f));
      }
      if (r == 0) {
        DAT_00690560[param_1] = 2;
      }
      break;
    case 2:
      DAT_006904a4[param_1] = DAT_006904a4[param_1] - 0.02f;
      if (DAT_006904a4[param_1] < 0.0f) {
        DAT_006904a4[param_1] = 0.0f;
        DAT_00690560[param_1] = 0;
      }
      break;
    default:
      return;
    }
    FUN_0045daf0(i, param_1);
    if (FUN_0040e350() != 6) {
      FUN_0045e0f0(i, 0.0f);
      return;
    }
    FUN_0045e0f0(i, DAT_006904a4[param_1]);
  }
}
