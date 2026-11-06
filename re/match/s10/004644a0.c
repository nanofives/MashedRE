// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x004644a0 (h7, by hand): 100%. DAT_0068f640 is an 8-byte record array (object pointer + dword).
   The last test reads DAT_006036d4[param_1] directly, not through the `cnt` pointer: through `cnt` the compiler
   threads the jl at 0x0046451a past the re-test at 0x004645a3. */
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
typedef struct Obj004644a0 {
  char pad0[0x30];
  int h30;
  unsigned char f34;
  char pad35[7];
  float f3c;
} Obj004644a0;
typedef struct { Obj004644a0 *p; int q; } Rec0068f640;
MGLOBAL(float, DAT_005cc55c, 0x005cc55c);
MGLOBAL(int, DAT_0060575c, 0x0060575c);
MGLOBALA(int, DAT_006036d4, 1, 0x006036d4);
MGLOBALA(Rec0068f640, DAT_0068f640, 1, 0x0068f640);
MGLOBALA(int, DAT_00690500, 1, 0x00690500);
void FUN_0045daf0(int p0, int p1);
void FUN_0045e0f0(int p0, float p1);
int FUN_0046cbb0(int p0, int *p1, int *p2);
int FUN_0046cc10(int *p0, int p1);
int FUN_0046d6d0(float *p0, int p1);
void FUN_005a66d0(Obj004644a0 *p0, int p1);
void FUN_005a6dc0(int p0, int p1, int p2, float p3);

// MATCH: 0x004644a0 FUN_004644a0
void FUN_004644a0(int param_1)
{
  int i;
  int *cnt;
  int a;
  int b;
  float v;

  FUN_0046cbb0(param_1, &a, &b);
  FUN_0046d6d0(&v, param_1);
  if (a != 2 || b <= 0) {
    cnt = &DAT_006036d4[param_1];
    FUN_0046cc10(cnt, param_1);
    if (*cnt == 0) {
      DAT_00690500[param_1] = 1;
    }
    if (DAT_00690500[param_1] == 1 && *cnt >= 2 && v > DAT_005cc55c) {
      i = DAT_0060575c + param_1;
      if ((DAT_0068f640[i].p->f34 & 2) == 0) {
        FUN_0045daf0(i, param_1);
        FUN_005a66d0(DAT_0068f640[i].p, 1);
        DAT_0068f640[i].p->f3c = 22050.0f;
        if (DAT_0068f640[i].p->h30 != 0) {
          FUN_005a6dc0(DAT_0068f640[i].p->h30, 4, 1, 22050.0f);
        }
        FUN_0045e0f0(i, 1.0f);
      }
      DAT_00690500[param_1] = 0;
    }
    if (DAT_006036d4[param_1] >= 2) {
      DAT_00690500[param_1] = 0;
    }
  }
}
