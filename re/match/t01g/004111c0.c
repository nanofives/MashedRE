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
   Extent (MATCH-RACEPATH-FRAME, 2026-11-09): Ghidra's body of 0x004111c0 is 1498 B in 6 ranges, because cases 1, 2,
     8, 9 and 11 tail-JMP into functions Ghidra has no entry for (0x0041125f -> 0x0040feb0, 0x00411272 -> 0x00410130,
     0x004112f2 -> 0x0040dd80, 0x004112fb -> 0x0040ddf0, 0x0041131e -> 0x00410ab0). Those five are the `// HIDDEN:`
     functions below (DDK only), each insn 100% at its Ghidra range and whole symbol BYTE-IDENTICAL; 355 + 46 + 16 +
     572 + 391 + 118 = 1498 (re/tools/match/extent_overrides.tsv, verify/matchracepath_20261109/frame/). What moved
     them: 0x0040feb0 / 0x00410130 use 0x0040fc00's case order (2, 6/10, 3/4/5, default) with case 3/4/5 falling
     into the default tail; 0x00410ab0 needs a volatile read of 0x0063b91c (else cl loads 0x007f100c first: fld/fadd
     operands swapped, caught by relocheck), `!(x < K)` (test ah,5 / jnp) and a non-static HELD 0x0040e460 defined
     before it (cl then keeps ECX/EDX live across the call at 0x00410b0c, as the original does; static gives it an
     EAX argument instead). 0x0040ffc2 calls the bare-RET stub 0x0045b350 with no argument, so the DDK side declares
     it without a prototype.
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
void FUN_0040de00(void);
int FUN_0040e350(void);
void FUN_0040e560(void);
void FUN_0040e590(void);
void FUN_00410860(void);
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
#ifndef MATCH_PORT
void FUN_0045b350();  /* bare RET stub; 0x0040ffc2 calls it with no argument */
#else
void FUN_0045b350(int p0);
#endif
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

/* ---- 0x004111c0 extent members (MATCH-RACEPATH-FRAME): the five functions 0x004111c0 tail-jumps to.
   DDK only (match.py / c4_reclass extent gate); the port build (MATCH_PORT) keeps the extern declarations. ---- */
#ifndef MATCH_PORT
typedef struct { int m_0; int m_4; int m_8; int m_c; } Rec7f1a14;
MGLOBAL(int, DAT_0063b90c, 0x0063b90c);
MGLOBAL(float, DAT_0063b91c, 0x0063b91c);
MGLOBAL(int, DAT_0063ba80, 0x0063ba80);
MGLOBAL(float, DAT_005cc574, 0x005cc574);
MGLOBAL(int, DAT_007f0fd8, 0x007f0fd8);
MGLOBALA(char, DAT_007f1041, 1, 0x007f1041);
MGLOBALA(char, DAT_007f1042, 1, 0x007f1042);
MGLOBALA(char, DAT_007f1501, 1, 0x007f1501);
MGLOBALA(char, DAT_007f1502, 1, 0x007f1502);
MGLOBALA(Rec7f1a14, DAT_007f1a14, 4, 0x007f1a14);
MGLOBAL(int, DAT_008a94d0, 0x008a94d0);

int FUN_004a2cbd(const char *fmt, ...);
void FUN_00401570(int p0);
void FUN_004046a0(void);
void FUN_004073b0(void);
void FUN_00407a60(void);
void FUN_0040b180(void);
void FUN_0040bd80(void);
void FUN_0040ddb0(void);
int FUN_0040e340(void);
MGLOBAL(int, DAT_0063b908, 0x0063b908);
// HELD: 0x0040e460 (body elsewhere, re/match/s02/0040e460.c; this copy exists only so the DDK knows the callee keeps
// ECX/EDX, which 0x00410ab0 relies on across its call at 0x00410b0c; not compiled in the port)
void FUN_0040e460(int param_1)
{
  DAT_0063b908 = param_1;
}
void FUN_0040e480(int p0, int p1);
void FUN_00414060(void);
void FUN_00414180(void);
void FUN_00414220(void);
void FUN_00418990(void);
void FUN_00419760(int p0, int p1);
void FUN_0041b4d0(void);
void FUN_0041b510(void);
void FUN_0041bf20(void);
void FUN_0041c010(void);
void FUN_0041cb80(void);
void FUN_0041cbc0(void);
void FUN_0041d730(void);
void FUN_0041d820(void);
void FUN_0041e130(int p0);
void FUN_0041eda0(int p0, int p1);
void FUN_0041f000(int p0, int *p1);
void FUN_00420d40(void);
void FUN_00422120(void);
void FUN_004222c0(void);
void FUN_00422b10(void);
void FUN_00425b10(void);
int FUN_00426c00(void);
void FUN_00426c10(int p0);
void FUN_00426c30(int p0);
void FUN_00426c70(int p0);
void FUN_0042aab0(void);
void FUN_004425d0(void);
void FUN_00458bf0(void);
void FUN_00459560(void);
void FUN_0045b9d0(void);
void FUN_0045ba00(int p0, int p1);
void FUN_0045be90(void);
void FUN_0045bed0(void);
void FUN_0046b1c0(int p0, int *p1);
void FUN_0046b540(int p0);
void FUN_0046c5c0(int p0);
void FUN_00471ac0(void);
void FUN_00471cf0(void);
void FUN_00475a60(void);
void FUN_00476430(void);
void FUN_00477730(void);
void FUN_00477b40(void);
void FUN_00480100(void);
void FUN_00484c90(void);
void FUN_0048f260(void);
void FUN_004904d0(void);

// HIDDEN: 0x0040dd80 _FUN_0040dd80 46
static void FUN_0040dd80(void)
{
  FUN_00414220();
  FUN_00426c70(**(int **)(PTR_PTR_005f2770 + DAT_0063ba78 * 4));
  FUN_0045bed0();
  DAT_0063ba8c = 2;
}

// HIDDEN: 0x0040ddf0 _FUN_0040ddf0 16
static void FUN_0040ddf0(void)
{
  FUN_0040ddb0();
  DAT_0063ba8c = 10;
}

// HIDDEN: 0x0040feb0 _FUN_0040feb0 572
static void FUN_0040feb0(void)
{
  int buf[6];
  int i;

  if (FUN_0042f6a0() == 0xb) {
    FUN_0040e480(0, 2);
    FUN_0040e480(1, 2);
    FUN_0040e480(2, 2);
    FUN_0040e480(3, 0);
  }
  DAT_0063ba80 = 0;
  FUN_00426c10(**(int **)(PTR_PTR_005f2770 + DAT_0063ba78 * 4));
  DAT_005f29b8 = 0xff;
  FUN_00422b10();
  DAT_007f101c = 0;
  DAT_007f0ff4 = 0;
  DAT_007f0ff8 = 0;
  FUN_004a2cbd("start of race\n");
  FUN_004425d0();
  FUN_00425b10();
  FUN_00459560();
  DAT_008a94d0 = 0;
  for (i = 0; i < 4; i++) {
    if (*(int *)(PTR_PTR_005f2770 + 0x34 + i * 4) == 0) {
      FUN_0041eda0(i, 0);
      FUN_0046c5c0(i);
      FUN_0045ba00(i, 4);
    }
    else {
      FUN_0041f000(i, buf);
      FUN_0046b1c0(i, buf);
      FUN_0046b540(i);
      FUN_0041eda0(i, 1);
      FUN_0045ba00(i, 1);
      DAT_008a94d0 = DAT_008a94d0 + 1;
    }
  }
  FUN_0040b180();
  FUN_0040bd80();
  FUN_0045b350();
  FUN_00414060();
  FUN_00484c90();
  FUN_0045b350(1);
  FUN_00477b40();
  FUN_00475a60();
  FUN_0045be90();
  FUN_00476430();
  FUN_00477730();
  FUN_00418990();
  FUN_00420d40();
  FUN_004222c0();
  switch (FUN_0042f6a0()) {
  case 2:
    FUN_0041e130(1);
    goto done;
  case 6:
  case 10:
    if (FUN_0042f500() != 0 && DAT_007f0fd0 != 2) {
      FUN_0041bf20();
      FUN_0041d730();
    }
    else {
      FUN_0041b4d0();
      FUN_0041cb80();
    }
    goto done;
  case 3:
  case 4:
  case 5:
    switch (DAT_007f0fd0) {
    case 7:
      FUN_00419760(0, 1);
      FUN_00401570(FUN_00426c00());
      break;
    case 10:
      FUN_004046a0();
    case 4:
    case 8:
    case 9:
      FUN_00401570(FUN_00426c00());
      break;
    case 5:
      break;
    default:
      if (FUN_0042f500() != 0) {
        FUN_0041bf20();
        FUN_0041d730();
      }
      else {
        FUN_0041b4d0();
        FUN_0041cb80();
      }
      break;
    }
  default:
    if (FUN_0042f500() != 0) {
      FUN_0041bf20();
      FUN_0041d730();
    }
    else {
      FUN_0041b4d0();
      FUN_0041cb80();
    }
    break;
  }
done:
  FUN_00458bf0();
  FUN_00407a60();
  FUN_00471cf0();
  DAT_005f29b8 = 100000;
  DAT_0063ba8c = 2;
}

// HIDDEN: 0x00410130 _FUN_00410130 391
static void FUN_00410130(void)
{
  FUN_00480100();
  FUN_00471ac0();
  FUN_0042aab0();
  if (DAT_007f0fd0 == 2) {
    DAT_007f0fd8 = DAT_007f0fd8 + 1;
    if (DAT_007f0fd8 < 0 || DAT_007f0fd8 >= FUN_0040e340()) {
      DAT_007f0fd8 = 0;
    }
  }
  FUN_0040e590();
  DAT_007f1014 = DAT_007f0ff4 > DAT_007f1018;
  FUN_00426c30(**(int **)(PTR_PTR_005f2770 + DAT_0063ba78 * 4));
  FUN_0040bd80();
  FUN_0045b9d0();
  FUN_00476430();
  FUN_00477730();
  FUN_00418990();
  FUN_00420d40();
  FUN_00422120();
  FUN_00477b40();
  switch (FUN_0042f6a0()) {
  case 2:
    goto done;
  case 6:
  case 10:
    if (FUN_0042f500() != 0 && DAT_007f0fd0 != 2) {
      FUN_0041c010();
      FUN_0041d820();
    }
    else {
      FUN_0041b510();
      FUN_0041cbc0();
    }
    goto done;
  case 3:
  case 4:
  case 5:
    switch (DAT_007f0fd0) {
    case 7:
      FUN_00419760(0, 1);
      break;
    case 10:
      FUN_004046a0();
      break;
    case 4:
    case 5:
    case 8:
    case 9:
      break;
    default:
      if (FUN_0042f500() != 0) {
        FUN_0041c010();
        FUN_0041d820();
      }
      else {
        FUN_0041b510();
        FUN_0041cbc0();
      }
      break;
    }
  default:
    if (FUN_0042f500() != 0) {
      FUN_0041c010();
      FUN_0041d820();
    }
    else {
      FUN_0041b510();
      FUN_0041cbc0();
    }
    break;
  }
done:
  FUN_00414180();
  FUN_004904d0();
  FUN_0048f260();
  FUN_004073b0();
  if (DAT_005f29b8 == 100000) {
    DAT_0063ba8c = 3;
  }
  else {
    DAT_0063ba8c = 4;
    DAT_005f29b8 = 12000;
    FUN_0040d470(1);
  }
  if (DAT_007f0fd0 == 6) {
    DAT_0063ba8c = 4;
  }
}

// HIDDEN: 0x00410ab0 _FUN_00410ab0 118
static void FUN_00410ab0(void)
{
  int i;
  int s;

  DAT_0063b91c = *(volatile float *)&DAT_0063b91c + DAT_007f100c;
  if (!(DAT_0063b91c < DAT_005cc574)) {
    for (i = 0; i < 4; i++) {
      s = DAT_007f1a14[i].m_0;
      if (s != -1 &&
          ((DAT_007f1042[s * 0x4c] != 0 && DAT_007f1502[s * 0x4c] == 0) ||
           (DAT_007f1041[s * 0x4c] != 0 && DAT_007f1501[s * 0x4c] == 0))) {
        DAT_0063ba8c = 9;
        FUN_0040e460(1);
        DAT_0063b90c = 0;
      }
    }
  }
}
#else
void FUN_0040dd80(void);
void FUN_0040ddf0(void);
void FUN_0040feb0(void);
void FUN_00410130(void);
void FUN_00410ab0(void);
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
