// FLAGS: /O2 /GS /Ob1
/* 0x004103a0 (360 B): 100% exact, regs 100%. Raw Ghidra 58.2% (in_EAX). The function is `static` with one parameter
   (delta) so cl 9176 passes it in EAX (pilot finding 1); the real caller is 0x004111c0 (call at 0x004112bb,
   mov eax,esi first), 1498 B and not in this file, so a small stand-in caller FUN_004103a0_caller sits below it
   (not the real TU, same caveat as the pilot 0x00419090/0x00499170). FUN_0046d7f0 takes (i, delta), the ebx push.
   Last step: setl->setg by writing DAT_007f0ff4 > DAT_007f1018. 0x007f0a04 is modelled as 12-byte records
   (fields +0/+4 written). No residual. Port note: a static body needs the real caller in the same .cpp, or the
   port keeps an extern wrapper. */
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
typedef struct { int m_0; int m_4; int m_8; } Rec7f0a04;
MGLOBAL(int, DAT_005f29b8, 0x005f29b8);
MGLOBAL(int, DAT_007f0ff4, 0x007f0ff4);
MGLOBAL(int, DAT_007f1018, 0x007f1018);
MGLOBAL(char *, PTR_PTR_005f2770, 0x005f2770);
MGLOBAL(uint, DAT_007f1014, 0x007f1014);
MGLOBAL(int, DAT_0063ba78, 0x0063ba78);
MGLOBAL(float, DAT_005ccdf4, 0x005ccdf4);
MGLOBAL(int, DAT_0063ba8c, 0x0063ba8c);
MGLOBALA(Rec7f0a04, DAT_007f0a04, 4, 0x007f0a04);
void FUN_0040e560(void);
void FUN_00411ae0(int p0, uint p1);
void FUN_00411d60(void);
void FUN_00418860(void);
void FUN_0041da90(float *p0);
void FUN_00426c90(int p0);
void FUN_00429840(int p0);
int FUN_00429860(void);
int FUN_0042f6a0(void);
int FUN_00430820(void);
int FUN_0046baa0(int p0);
int FUN_0046c730(int p0);
int FUN_0046c750(int p0);
int FUN_0046c7b0(int p0);
int FUN_0046d780(int p0);
int FUN_0046d7f0(int p0, int p1);

// MATCH: 0x004103a0 FUN_004103a0
static void FUN_004103a0(int delta)
{
  float t;
  int i;

  t = 0.0f;
  if (FUN_00429860() != 0 && FUN_00430820() == 0 && FUN_0042f6a0() == 2) {
    FUN_0040e560();
  }
  DAT_005f29b8 = DAT_005f29b8 - delta;
  DAT_007f1014 = DAT_007f0ff4 > DAT_007f1018;
  FUN_00426c90(**(int **)(PTR_PTR_005f2770 + DAT_0063ba78 * 4));
  FUN_00418860();
  for (i = 0; i < 4; i++) {
    if (*(int *)(PTR_PTR_005f2770 + 0x34 + i * 4) != 0 && FUN_0046c7b0(i) == 1) {
      FUN_0046baa0(i);
      FUN_0046d7f0(i, delta);
    }
  }
  FUN_0041da90(&t);
  if (t >= DAT_005ccdf4) {
    for (i = 0; i < 4; i++) {
      if (*(int *)(PTR_PTR_005f2770 + 0x34 + i * 4) != 0 && FUN_0046c7b0(i) == 1) {
        FUN_0046d780(i);
        DAT_007f0a04[i].m_0 = FUN_0046c750(i);
        DAT_007f0a04[i].m_4 = FUN_0046c730(i);
      }
    }
    DAT_005f29b8 = 0;
    if (FUN_0042f6a0() == 2) {
      FUN_00429840(1);
      FUN_00411d60();
    }
    DAT_0063ba8c = 6;
  }
  if (FUN_0042f6a0() == 2) {
    FUN_00411ae0(0, 0);
  }
}

/* stand-in caller (not the real TU): makes cl pass delta in EAX, as 0x004112b5 does */
void FUN_004103a0_caller(int delta)
{
  FUN_004103a0(delta);
}
