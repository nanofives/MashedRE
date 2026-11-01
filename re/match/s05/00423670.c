// FLAGS: /O2 /GS /Ob1
/* 0x00423670 (1148 B). Best 100.0% exact (regs 100, ops 100).
   Raw g2s 33.4%. Written from the disassembly. What moved it:
   - switch on DAT_007f1a68 with case 0 and default as separate (identical) arms: the 4-entry table at
     0x00423aec maps 0 to the default block (case 0 folded into default gives a dec chain, no table);
   - the 0x204-byte spline record {float x, z}[64] + count at +0x200, and one record Ed at 0x007f1a98 holding
     the selection index, the two int[0x2000] arrays at 0x007f1a9c / 0x007f9a9c, the dword 0x00801a9c and the
     four Spline[3] groups 0x00801aa0 / 0x008020ac / 0x008026b8 / 0x00802cc4: with separate globals cl proves
     sp cannot alias the index and drops the reload of count after `sel++` (70.0% vs 64.6%);
   - mode 0 sub-items as an if chain in the original order 0,1,2,3,8,7,5 with the item read into a local
     (the original stores the compared register as DAT_007f1a68);
   - the shift-up loop field by field (a struct copy addresses from &p[i] instead of &p[i-1].z);
   - the 0x006440e8 repeat ladder with ONE shared `DAT_007f1a70 = 1` after the if/else chain: written three
     times, cl 9176 keeps 1 in edi for the whole function (push edi, 70.0%); shared, cl tail-duplicates it and
     reuses ecx/eax/imm exactly as the original.
   Residual: none. */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
typedef struct { float x, z; } SplinePt;
typedef struct { SplinePt p[64]; int n; } Spline;
typedef struct { int sel; int a[0x2000]; int b[0x2000]; int n; Spline s0[3]; Spline s1[3]; Spline s2[3]; Spline s3[3]; } Ed;
typedef struct { float x, y, z; } V3;
MGLOBAL(Ed, DAT_007f1a98, 0x007f1a98);
MGLOBAL(int, DAT_007f1a68, 0x007f1a68);
MGLOBAL(int, DAT_007f1a6c, 0x007f1a6c);
MGLOBAL(int, DAT_007f1a70, 0x007f1a70);
MGLOBAL(int, DAT_007f1a54, 0x007f1a54);
MGLOBAL(int, DAT_007f1a64, 0x007f1a64);
MGLOBAL(int, DAT_007f1a74, 0x007f1a74);
MGLOBAL(float, DAT_007f1a58, 0x007f1a58);
MGLOBAL(float, DAT_007f1a5c, 0x007f1a5c);
MGLOBAL(int, DAT_00644100, 0x00644100);
MGLOBAL(int, DAT_006440e8, 0x006440e8);
MGLOBAL(char, DAT_007f1043, 0x007f1043);
MGLOBAL(char, DAT_007f1503, 0x007f1503);
MGLOBAL(char, DAT_007f1072, 0x007f1072);
MGLOBAL(char, DAT_007f1532, 0x007f1532);
MGLOBAL(char, DAT_007f1073, 0x007f1073);
MGLOBAL(char, DAT_007f1533, 0x007f1533);
MGLOBAL(char, DAT_007f1075, 0x007f1075);
MGLOBAL(char, DAT_007f1535, 0x007f1535);
MGLOBAL(char, DAT_007f1074, 0x007f1074);
MGLOBAL(char, DAT_007f1534, 0x007f1534);
MGLOBAL(int, DAT_00803324, 0x00803324);
void FUN_00408b00(int a, int b, int c, V3 *v, float *f);
int FUN_00423540(void);
void FUN_00423630(void);
int FUN_0046d4a0(float **m, int b);
int FUN_004704c0(int a, V3 v, float f, void *p);

// MATCH: 0x00423670 FUN_00423670
void FUN_00423670(void)
{
  Spline *sp;
  float f;
  float *m;
  V3 v;
  int i;

  switch (DAT_007f1a68) {
  case 1: sp = &DAT_007f1a98.s1[DAT_007f1a6c]; break;
  case 2: sp = &DAT_007f1a98.s2[DAT_007f1a6c]; break;
  case 3: sp = &DAT_007f1a98.s3[DAT_007f1a6c]; break;
  case 0: sp = &DAT_007f1a98.s0[DAT_007f1a6c]; break;
  default: sp = &DAT_007f1a98.s0[DAT_007f1a6c]; break;
  }
  DAT_007f1a70 = 0;
  if (DAT_007f1043 && !DAT_007f1503) {
    if (DAT_007f1a54 == 0) {
      i = DAT_007f1a64;
      if (i == 0) { DAT_007f1a54 = 6; DAT_007f1a64 = 2; DAT_007f1a68 = i; return; }
      if (i == 1) { DAT_007f1a54 = 6; DAT_007f1a64 = 2; DAT_007f1a68 = i; return; }
      if (i == 2) { DAT_007f1a54 = 6; DAT_007f1a64 = 2; DAT_007f1a68 = i; return; }
      if (i == 3) { DAT_007f1a54 = 6; DAT_007f1a64 = 2; DAT_007f1a68 = i; return; }
      if (DAT_007f1a64 == 8) { DAT_007f1a54 = 4; DAT_007f1a64 = 2; DAT_007f1a74 = 0; return; }
      if (DAT_007f1a64 == 7) { DAT_007f1a54 = 1; DAT_007f1a64 = 2; return; }
      if (DAT_007f1a64 == 5) { DAT_007f1a54 = 3; DAT_007f1a64 = 2; return; }
      return;
    }
    if (DAT_007f1a54 == 1) {
      if (DAT_007f1a64 == 2) { DAT_007f1a54 = 0; DAT_007f1a64 = 0; return; }
      FUN_00423630();
      return;
    }
    if (DAT_007f1a54 == 3) {
      if (DAT_007f1a64 != 2) FUN_00423540();
      DAT_007f1a54 = 0; DAT_007f1a64 = 0;
      return;
    }
    if (DAT_007f1a54 == 4) {
      if (DAT_007f1a64 == 4) {
        DAT_007f1a74 = (DAT_007f1a74 - 1) & 1;
      } else {
        if (DAT_007f1a64 == 3) {
          FUN_00408b00(0, 0, 1, &v, &f);
          FUN_004704c0(0, v, f, &DAT_00803324);
        }
        if (DAT_007f1a64 == 2) { DAT_007f1a54 = 0; DAT_007f1a64 = 0; return; }
      }
    }
    if (DAT_007f1a54 == 6) {
      if (DAT_007f1a64 == 2) {
        sp->p[DAT_007f1a98.sel].x = DAT_007f1a58 + 1.0f;
        sp->p[DAT_007f1a98.sel].z = DAT_007f1a5c - 1.0f;
        if (DAT_007f1a98.sel == sp->n) sp->n++;
      }
      if (DAT_007f1a64 == 3 && sp->n != 0x40) {
        for (i = sp->n + 1; i > DAT_007f1a98.sel; i--) { sp->p[i].x = sp->p[i - 1].x; sp->p[i].z = sp->p[i - 1].z; }
        sp->p[DAT_007f1a98.sel].x = DAT_007f1a58 + 1.0f;
        sp->p[DAT_007f1a98.sel].z = DAT_007f1a5c - 1.0f;
        if (DAT_007f1a98.sel == sp->n) DAT_007f1a98.sel++;
        sp->n++;
      }
      if (DAT_007f1a64 == 5) { DAT_007f1a54 = 0; DAT_007f1a64 = 0; return; }
    }
  }
  if (DAT_007f1a54 == 4) {
    FUN_0046d4a0(&m, 0);
    DAT_007f1a58 = m[12];
    DAT_007f1a5c = m[14];
    if (DAT_007f1a64 == 5) {
      if (DAT_007f1043) {
        DAT_00644100++;
        if (DAT_006440e8 == 0) {
          if (DAT_00644100 != 1) goto sel;
          DAT_006440e8 = 1;
        } else if (DAT_006440e8 == 1) {
          if (DAT_00644100 != 0x1e) goto sel;
          DAT_006440e8 = 2;
        } else {
          if (DAT_006440e8 != 2 || DAT_00644100 != 3) goto sel;
        }
        DAT_007f1a70 = 1;
      } else
        DAT_006440e8 = 0;
      DAT_00644100 = 0;
    }
  }
sel:
  if (DAT_007f1a54 == 6) {
    if (DAT_007f1072 && !DAT_007f1532) DAT_007f1a98.sel++;
    if (DAT_007f1073 && !DAT_007f1533) DAT_007f1a98.sel--;
    if (DAT_007f1a98.sel < 0) DAT_007f1a98.sel = sp->n;
    if (DAT_007f1a98.sel > sp->n || DAT_007f1a98.sel == 0x40) DAT_007f1a98.sel = 0;
    if (DAT_007f1075 && !DAT_007f1535) DAT_007f1a6c++;
    if (DAT_007f1074 && !DAT_007f1534) DAT_007f1a6c--;
    if (DAT_007f1a6c < 0) DAT_007f1a6c = 2;
    else if (DAT_007f1a6c > 2) DAT_007f1a6c = 0;
  }
}
