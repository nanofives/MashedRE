// FLAGS: /O2 /GS /Ob1
/* re/match/t01g/004111c0.c -- one TU slice: 0x0040fc00 (Race::Tick), 0x004102f0, 0x004103a0, 0x004111c0 (race frame).
   0x0040fc00 (621 B): 100.0% exact, regs 100.0%, whole symbol (684 B incl. its two jump tables + byte index table)
     BYTE-IDENTICAL. Raw g2s 59.4% (unaff_ESI). Held as a static callee, parameter `uint delta`, so 0x004111c0's
     calls pass it in ESI. What moved it: (1) the 1/3000 factor at 0x005cc948 is a global float read through memory,
     not a literal, which stops cl from CSE-ing the two `delta * K` products; (2) `(float)delta` written at each use,
     not a named local, so the conversion is CSE'd into [esp] the way the original keeps it (fst [esp] then fmul);
     (3) case order 2, 6/10, 3/4/5, 11, default; inner switch case 5 before 4/7/8/9 (the inner jump table pins it).
     The tail Ghidra lost: DAT_007f1030 = DAT_007f1030 + (float)delta * 1000.0f (ftol 0x004a2c48).
   0x004111c0 (400 B = 356 B code + 11-entry jump table at 0x00411324): 84.7% by instruction (the score counts the
     table bytes as instructions), whole symbol BYTE-IDENTICAL (match.py --bytes). DAT_007f0fc8 is a float (camera
     jitter amplitude, 0.0 live): as int cl caches the 0 in ESI and pushes ESI before the first block.
   Held static callees (all 100%, BYTE-IDENTICAL): 0x0040fc00 (arg in ESI), 0x004102f0 (arg in EBX, source from
     re/match/s02/004102f0.c), 0x004103a0 (arg in EAX, source from re/match/s02/004103a0.c). PTR_PTR_005f2770 is
     `char *` here for both; 0x004102f0's index written as *(int *)(PTR + 0x34 + i * 4), still 100%.
   Residual: none. */
/* ---- MATCH-S07 preamble (same text in every re/match/s07 source; not part of any function body) ----
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
#ifdef __cplusplus
#define MATCH_STATIC
#else
#define MATCH_STATIC static
#endif
typedef struct { int m_0; int m_4; int m_8; } Rec7f0a04;
MGLOBAL(int, DAT_005f29b8, 0x005f29b8);
MGLOBAL(int, DAT_005f29c0, 0x005f29c0);
MGLOBAL(char *, PTR_PTR_005f2770, 0x005f2770);
MGLOBAL(int, DAT_0063ba78, 0x0063ba78);
MGLOBAL(int, DAT_0063ba8c, 0x0063ba8c);
MGLOBAL(int, DAT_0066d728, 0x0066d728);
MGLOBALA(Rec7f0a04, DAT_007f0a04, 4, 0x007f0a04);
MGLOBAL(float, DAT_007f0fc8, 0x007f0fc8);
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);
MGLOBAL(int, DAT_007f0fec, 0x007f0fec);
MGLOBAL(int, DAT_007f0ff4, 0x007f0ff4);
MGLOBAL(int, DAT_007f0ff8, 0x007f0ff8);
MGLOBAL(int, DAT_007f1000, 0x007f1000);
MGLOBAL(int, DAT_007f1004, 0x007f1004);
MGLOBAL(uint, DAT_007f1008, 0x007f1008);
MGLOBAL(float, DAT_007f100c, 0x007f100c);
MGLOBAL(float, DAT_007f1010, 0x007f1010);
MGLOBAL(uint, DAT_007f1014, 0x007f1014);
MGLOBAL(int, DAT_007f1018, 0x007f1018);
MGLOBAL(int, DAT_007f101c, 0x007f101c);
MGLOBAL(uint, DAT_007f1030, 0x007f1030);
MGLOBAL(float, DAT_005ccdf4, 0x005ccdf4);
MGLOBAL(float, DAT_005cc948, 0x005cc948);
MGLOBAL(float, DAT_005cc9fc, 0x005cc9fc);

void FUN_00403250(int p0);
void FUN_004039f0(void);
void FUN_00406ce0(void);
void FUN_0040ce00(void);
void FUN_0040d470(int p0);
void FUN_0040da50(uint p0);
void FUN_0040dba0(void);
void FUN_0040dbd0(uint p0);
void FUN_0040dd80(void);
void FUN_0040ddf0(void);
void FUN_0040de00(void);
int FUN_0040e350(void);
void FUN_0040e560(void);
void FUN_0040e590(void);
void FUN_0040feb0(void);
void FUN_00410130(void);
void FUN_00410860(void);
void FUN_00410ab0(void);
void FUN_00411170(uint p0);
void FUN_00411ae0(int p0, uint p1);
void FUN_00411d60(void);
void FUN_00412f30(int p0);
void FUN_00413f20(void);
void FUN_00418860(void);
void FUN_004189a0(void);
void FUN_0041a250(void);
void FUN_0041b540(void);
void FUN_0041c090(void);
void FUN_0041c180(void);
void FUN_0041cc50(void);
void FUN_0041d830(void);
void FUN_0041d930(void);
void FUN_0041da90(float *p0);
void FUN_0041dca0(void);
void FUN_0041e6c0(void);
void FUN_004215a0(void);
void FUN_004222d0(void);
int FUN_00425a40(uint p0);
void FUN_00425d50(uint p0);
void FUN_00426630(int p0);
void FUN_004266f0(int p0);
void FUN_00426ab0(int p0, uint p1);
int FUN_00426ba0(void);
void FUN_00426c90(int p0);
void FUN_004292c0(int *p0, int p1);
void FUN_004292d0(int *p0, int *p1);
void FUN_00429310(void);
void FUN_00429840(int p0);
int FUN_00429860(void);
void FUN_004430a0(int p0);
int FUN_004430b0(void);
int FUN_00405460(uint p0);
int FUN_0042f500(void);
int FUN_0042f6a0(void);
int FUN_00430820(void);
void FUN_00448700(int p0, int p1);
void FUN_0045b350(int p0);
void FUN_0045bba0(void);
int FUN_00467210(int p0);
int FUN_0046baa0(int p0);
int FUN_0046c730(int p0);
int FUN_0046c750(int p0);
int FUN_0046c7b0(int p0);
int FUN_0046d780(int p0);
int FUN_0046d7f0(int p0, int p1);
void FUN_00475c10(void);
void FUN_00476440(void);
void FUN_004777d0(void);
void FUN_00477920(void);
void FUN_00485070(void);
void FUN_00488e70(int p0);
void FUN_00490380(void);
void FUN_00490500(void);
int FUN_00496930(int p0);
int FUN_0055dec0(int *p0);

// MATCH: 0x0040fc00 FUN_0040fc00
MATCH_STATIC void FUN_0040fc00(uint delta)
{
  float secs;

  secs = (float)delta * DAT_005cc948;
  if (FUN_00496930(6) != 0) {
    DAT_005f29c0 = 1;
  }
  if (FUN_00496930(5) != 0) {
    DAT_005f29c0 = 0;
  }
  DAT_007f1008 = delta;
  DAT_007f100c = secs;
  if (DAT_005f29c0 != 0) {
    FUN_00425a40(delta);
  }
  FUN_0040da50(delta);
  FUN_0040d470(0);
  FUN_00426ab0(**(int **)(PTR_PTR_005f2770 + DAT_0063ba78 * 4), delta);
  FUN_00412f30(FUN_00467210(0));
  FUN_00488e70(FUN_00467210(0));
  FUN_0040ce00();
  FUN_00425d50(delta);
  if (DAT_007f0fd0 == 5) {
    FUN_00406ce0();
  }
  FUN_0045bba0();
  FUN_00475c10();
  FUN_00476440();
  FUN_004777d0();
  FUN_00477920();
  FUN_004189a0();
  FUN_004215a0();
  FUN_004222d0();
  FUN_00490500();
  FUN_00490380();
  FUN_00429310();
  FUN_00485070();
  FUN_0040dba0();
  FUN_0041d930();
  if (DAT_0063ba8c == 7 || (DAT_0063ba8c > 9 && DAT_0063ba8c <= 11)) {
    FUN_0045b350(0);
    FUN_00413f20();
  }
  else {
    FUN_0045b350(1);
  }
  switch (FUN_0042f6a0()) {
  case 2:
    FUN_0041e6c0();
    break;
  case 6:
  case 10:
    if (FUN_0042f500() != 0 && DAT_007f0fd0 != 2) {
      FUN_0041c090();
      FUN_0041d830();
    }
    else {
      FUN_0041b540();
      FUN_0041cc50();
    }
    break;
  case 3:
  case 4:
  case 5:
    switch (DAT_007f0fd0) {
    case 5:
      FUN_0041c180();
      break;
    case 4:
    case 7:
    case 8:
    case 9:
      FUN_0041dca0();
      break;
    case 10:
      FUN_0041a250();
      if (FUN_0040e350() == 6) {
        FUN_004039f0();
      }
      break;
    default:
      if (FUN_0042f500() != 0) {
        FUN_0041c090();
        FUN_0041d830();
      }
      else {
        FUN_0041b540();
        FUN_0041cc50();
      }
      break;
    }
    break;
  case 11:
    FUN_00403250(DAT_007f1004);
  default:
    if (FUN_0042f500() != 0) {
      FUN_0041c090();
      FUN_0041d830();
    }
    else {
      FUN_0041b540();
      FUN_0041cc50();
    }
    break;
  }
  if (DAT_007f1000 > 0x3c) {
    DAT_007f1010 = DAT_007f1010 + (float)delta * DAT_005cc948;
    DAT_007f1030 = DAT_007f1030 + (float)delta * 1000.0f;
  }
  DAT_007f0fec = DAT_007f0fec + delta;
  if (DAT_007f0fec > 0xf731400) {
    DAT_007f0fec = 0xf731400;
  }
  if (DAT_0063ba8c != 7) {
    DAT_007f0ff4 = DAT_007f0ff4 + delta;
    DAT_007f0ff8 = DAT_007f0ff8 + delta;
  }
}

// MATCH: 0x004102f0 FUN_004102f0
MATCH_STATIC void FUN_004102f0(uint param_1)
{
  int i;

  if (DAT_005f29b8 == 100000) {
    FUN_00448700(0, 0);
    FUN_004430a0(1);
    FUN_0040e590();
    FUN_0040d470(1);
  }
  for (i = 0; i < 4; i++) {
    if (*(int *)(PTR_PTR_005f2770 + 0x34 + i * 4) != 0 && FUN_0046c7b0(i) == 1) {
      FUN_0046baa0(i);
    }
  }
  DAT_005f29b8 = DAT_005f29b8 - (param_1 * 0x44c) / 0x3c;
  if (FUN_00405460(param_1) == 0 && FUN_004430b0() == 0) {
    DAT_0063ba8c = 4;
    DAT_005f29b8 = 12000;
  }
}

// HELD: 0x004103a0 (body elsewhere; this copy exists only so the DDK compiles the caller as the original; not compiled in the port)
#ifndef MATCH_PORT
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
#else
void FUN_004103a0(int delta);
#endif

// MATCH: 0x004111c0 FUN_004111c0
void FUN_004111c0(uint param_1)
{
  int msg;
  int r;

  DAT_007f101c = DAT_007f101c + 1;
  if (FUN_00426ba0() != 0) {
    r = FUN_0055dec0(&DAT_0066d728);
    if (r == 5) {
      FUN_004292d0(&DAT_0066d728, &msg);
      FUN_004292c0(&DAT_0066d728, 0);
      FUN_00426630(msg);
    }
    else if (r == 6) {
      FUN_004292d0(&DAT_0066d728, &msg);
      FUN_004292c0(&DAT_0066d728, 0);
      FUN_004266f0(msg);
    }
    switch (DAT_0063ba8c) {
    case 1:
      DAT_007f0fc8 = 0.0f;
      FUN_0040feb0();
      break;
    case 2:
      DAT_007f0fc8 = 0.0f;
      FUN_00410130();
      break;
    case 3:
      DAT_007f0fc8 = 0.0f;
      FUN_004102f0(param_1);
      FUN_0040fc00(param_1);
      break;
    case 4:
      DAT_007f0fc8 = 0.0f;
      FUN_0040dbd0(param_1);
      FUN_0040fc00(param_1);
      break;
    case 5:
      FUN_004103a0(param_1);
      FUN_0040fc00(param_1);
      break;
    case 6:
      FUN_00411170(param_1);
      FUN_0040fc00(param_1);
      break;
    case 7:
      FUN_00410860();
      FUN_0040fc00(param_1);
      break;
    case 8:
      FUN_0040dd80();
      break;
    case 9:
      FUN_0040ddf0();
      break;
    case 10:
      FUN_0040de00();
      FUN_0040fc00(param_1);
      break;
    case 11:
      FUN_0040fc00(param_1);
      FUN_00410ab0();
      break;
    }
  }
}
