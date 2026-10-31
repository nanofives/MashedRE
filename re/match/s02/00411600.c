// FLAGS: /O2 /GS /Ob1
/* 0x00411600 FUN_00411600 (334 B): 100.0% exact, regs 100.0%.
   Raw g2s was 99.0% (cmp eax,2 vs sub eax,2): the call result is a switch with one case 2.
   Residual: none. */
/* ---- MATCH-S02 preamble (same text in every re/match/s02 source; not part of any function body) ----
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
MGLOBAL(int, DAT_0063bb14, 0x0063bb14);
MGLOBAL(int, DAT_0063bb10, 0x0063bb10);
MGLOBAL(undefined4, DAT_007f0ff4, 0x007f0ff4);
MGLOBAL(undefined4, DAT_0063bb20, 0x0063bb20);

int FID_conflict__wprintf(const char *fmt, ...);
int FUN_00407a00(int p0, int p1);
void FUN_00411350(int p0, float *p1, int *p2, int *p3);
int FUN_0042f6a0(void);
undefined4 FUN_0046d4a0(int *p0, uint p1);
void FUN_004829d0(int p0, int p1, int p2, int p3);
void FUN_004987b0(const char *fmt, ...);

// MATCH: 0x00411600 FUN_00411600
void FUN_00411600(int param_1)
{
  int t;
  float sec;
  int sub;
  int mn;
  int local_4;

  switch (FUN_0042f6a0()) {
  case 2:
    if (DAT_0063bb14 == DAT_0063bb10) {
      FUN_004987b0("Time trial recording error time %d\n", DAT_007f0ff4);
    }
    FUN_0046d4a0(&local_4, 0);
    FUN_004829d0(DAT_0063bb14, local_4, param_1, 10);
    DAT_0063bb20 = 0;
    if (FUN_00407a00(0, 0) != 0 && *(int *)(DAT_0063bb14 + 0x17c) == 0) {
      *(int *)(DAT_0063bb14 + 0x17c) = param_1;
      t = *(int *)(DAT_0063bb14 + 0x17c);
      FUN_00411350(t, &sec, &sub, &mn);
      FID_conflict__wprintf("Hit intermediate 0 %d\n", t);
      FID_conflict__wprintf("Hit intermediate 0 %d:%d:%f\n", mn, sub, (double)sec);
    }
    if (FUN_00407a00(0, 1) != 0 && *(int *)(DAT_0063bb14 + 0x180) == 0) {
      *(int *)(DAT_0063bb14 + 0x180) = param_1;
      t = *(int *)(DAT_0063bb14 + 0x180);
      FUN_00411350(t, &sec, &sub, &mn);
      FID_conflict__wprintf("Hit intermediate 1 %d\n", t);
      FID_conflict__wprintf("Hit intermediate 1 %d:%d:%f\n", mn, sub, (double)sec);
    }
    break;
  }
}
