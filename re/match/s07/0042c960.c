// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x0042c960 FUN_0042c960 (CameraTransitionStateMachine per L-72; 2286 B): 100% by hand (raw 24.5%).
   What moved it (raw 24.5 -> 51 -> 62 -> 80 -> 89 -> 96 -> 100):
   - Script switch: case bodies in source order 5,4,1,2,6,7,3 (laid out verbatim). Case 5 written
     `if (FUN_00429870() == 0) { DAT_0067ea08++; p++; break; } p++; <body>` so its body cross-jumps into case 1's
     identical tail at 0x0042ca54, and its else tail shares case 4's `add esi,4` at 0x0042ca37.
   - 0x0067ebc8 is one struct {int n; int t; int a[20]; float h;} (n = 0x0067ebc8, t = 0x0067ebcc, a = 0x0067ebd0,
     h = 0x0067ec20): a store to a[n] may alias n, which gives the original's `inc dword ptr [n]` reloads.
   - Loop body: `if (DAT_0067ea64 != 0) {m0 path} else {m4 path}`; the m0 path stores `v + 0xa4` without
     reassigning v (register allocation of p/k/a in ebp/ebx); loop-tail sum split as `f = call + h; h = f + 8.0f;`
     with `n += 2` last (gives `add dword ptr [n], 2`).
   - Case 3's third argument as an if/else of two calls (cross-jumped push 0 / push 1); state 2's countdown as
     `if (ed68 >= 20) ed68 -= 20; else ed68 = 0;`; the state switch on a local `st` (the original jumps straight to
     the jump table after forcing state 3).
   - First loop `for (i = DAT_007f1000; i > 0; i -= 50)` (trip count (n-1)/50+1).
   Constants: 0x3f333333 = 0.7f, 0x005cc9f4 = 8.0f, 0x005cc750 = 16.0f, 0x005cc728 = 60.0f, 0x005cc31c = 3.0f,
   0x005cc320 = 1.0f, 0x005cc35c = 4.0f, 0x005d757c = 0.0f, 0x005cd274 = 40.0f. Tables at 0x0042d250 (7 cases) and
   0x0042d26c (6 states) follow the function. 0x007f1a18 is a 16-byte record array (fields +0 and +4 read).
   Array lengths in MGLOBALA declarations are [UNCERTAIN] (only indexed reads/writes here). Residual: none. */
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
MGLOBAL(int, DAT_007f1000, 0x007f1000);
MGLOBAL(float, DAT_007f1004, 0x007f1004);
MGLOBAL(int, DAT_0067e7c8, 0x0067e7c8);
MGLOBAL(int, DAT_0067e7cc, 0x0067e7cc);
MGLOBAL(int, DAT_0067e9fc, 0x0067e9fc);
MGLOBAL(int, DAT_0067ea08, 0x0067ea08);
MGLOBAL(int, DAT_0067ea64, 0x0067ea64);
MGLOBAL(int, DAT_0067ea6c, 0x0067ea6c);
MGLOBALA(int, DAT_0067eae0, 4, 0x0067eae0);
MGLOBAL(float, DAT_0067ebb0, 0x0067ebb0);
MGLOBAL(float, DAT_0067ebb4, 0x0067ebb4);
MGLOBAL(float, DAT_0067ebb8, 0x0067ebb8);
MGLOBAL(float, DAT_0067ebbc, 0x0067ebbc);
typedef struct { int n; int t; int a[20]; float h; } Seq0067ebc8;
MGLOBAL(Seq0067ebc8, DAT_0067ebc8, 0x0067ebc8);
MGLOBAL(float, DAT_0067ec24, 0x0067ec24);
MGLOBAL(int, DAT_0067ec28, 0x0067ec28);
MGLOBAL(int, DAT_0067ecdc, 0x0067ecdc);
MGLOBAL(int, DAT_0067ed4c, 0x0067ed4c);
MGLOBAL(int, DAT_0067ed68, 0x0067ed68);
MGLOBAL(float, DAT_0067f0d4, 0x0067f0d4);
MGLOBAL(int, DAT_005f76c4, 0x005f76c4);
MGLOBAL(int, DAT_005f76c8, 0x005f76c8);
MGLOBALA(int *, PTR_DAT_005f7854, 22, 0x005f7854);
MGLOBALA(int *, PTR_DAT_005f78ac, 22, 0x005f78ac);
typedef struct { int m0; int m4; int m8; int mc; } Rec007f1a18;
MGLOBALA(Rec007f1a18, DAT_007f1a18, 16, 0x007f1a18);
void FUN_004111c0(int p0);
void FUN_00429840(int p0);
void FUN_0042c510(void);
int FUN_00429870(void);
void FUN_00424270(int *p0, int p1, int p2);
int FUN_0040e340(void);
void FUN_0042c7c0(int p0);
float FUN_004282a0(int p0, float p1);
int FUN_00423b40(int p0);
int FUN_00423b60(int p0);
int FUN_00423b80(int p0);
int FUN_00423ba0(int p0);
int FUN_00423bc0(int p0);
int FUN_00423c40(int p0);
int FUN_00423cc0(int p0);
int FUN_00423d50(int p0);
int FUN_00423dd0(int p0);
int FUN_00423e60(int p0);
int FUN_00423ee0(int p0);
int FUN_00423f60(int p0);
int FUN_00423ff0(int p0);
int FUN_00424070(int p0);
int FUN_00424100(int p0);
int FUN_004241c0(int p0);
void FUN_0042c300(int p0, int p1);
void FUN_0042c3c0(int p0, int p1, int p2, int p3);
void FUN_0040b460(int *p0);
void FUN_0040b540(int *p0);
int FUN_0040b6b0(int p0);
int FUN_0040b6c0(int p0);
// MATCH: 0x0042c960 FUN_0042c960
void FUN_0042c960(void)
{
  int i;
  int *p;
  int n;
  int k;
  int v;
  int a;
  int b;
  int flag;
  int st;
  float f;

  for (i = DAT_007f1000; i > 0; i -= 50) {
    FUN_004111c0(50);
  }
  if (DAT_0067e7c8 == 1) {
    if (DAT_0067e9fc == 2) {
      FUN_00429840(0);
    }
    if (DAT_0067ec28 != 0) {
      FUN_0042c510();
      DAT_0067ec28 = 0;
    }
    if (DAT_0067ebc8.n == 0) {
      DAT_0067ebc8.t = 0x8e8;
      if (DAT_0067e9fc == 2) {
        p = PTR_DAT_005f78ac[DAT_0067ea08];
      }
      else {
        p = PTR_DAT_005f7854[DAT_0067ea08];
      }
      for (; *p != -1; p++) {
        switch (*p) {
        case 5:
          if (FUN_00429870() == 0) {
            DAT_0067ea08++;
            p++;
            break;
          }
          p++;
          DAT_0067ebc8.a[DAT_0067ebc8.n] = *p;
          DAT_0067ebc8.n++;
          DAT_0067ebc8.h = FUN_004282a0(*p, 0.7f) + DAT_0067ebc8.h + 8.0f;
          break;
        case 4:
          p++;
          break;
        case 1:
          p++;
          if (DAT_0067e9fc == 2) {
            DAT_0067ebc8.a[DAT_0067ebc8.n] = *p;
            DAT_0067ebc8.n++;
            DAT_0067ebc8.h = FUN_004282a0(*p, 0.7f) + DAT_0067ebc8.h + 8.0f;
          }
          break;
        case 2:
          p++;
          FUN_00424270(DAT_0067eae0, *p, 0);
          n = FUN_0040e340();
          if (DAT_0067ea64 != 0) {
            n = 2;
          }
          for (i = 0; i < n; i++) {
            if (DAT_0067ea64 != 0) {
              v = DAT_007f1a18[DAT_0067eae0[i]].m0;
              FUN_0042c7c0(v);
              DAT_0067ebc8.a[DAT_0067ebc8.n] = v + 0xa4;
              DAT_0067ebc8.h = FUN_004282a0(v + 0xa4, 0.7f) + DAT_0067ebc8.h + 8.0f;
            }
            else {
              v = DAT_007f1a18[DAT_0067eae0[i]].m4 + 0x79;
              DAT_0067ebc8.a[DAT_0067ebc8.n] = v;
              DAT_0067ebc8.h = FUN_004282a0(v, 0.7f) + DAT_0067ebc8.h + 8.0f;
            }
            if (*p == 0) b = FUN_00423b40(DAT_0067eae0[i]);
            if (*p == 1) b = FUN_00423b60(DAT_0067eae0[i]);
            if (*p == 4) b = FUN_00423cc0(DAT_0067eae0[i]);
            if (*p == 6) b = FUN_00423dd0(DAT_0067eae0[i]);
            if (*p == 7) b = FUN_00423b80(DAT_0067eae0[i]);
            if (*p == 8) b = FUN_00423ba0(DAT_0067eae0[i]);
            if (*p == 0xb) b = FUN_00423f60(DAT_0067eae0[i]);
            if (*p == 0xd) b = FUN_00424070(DAT_0067eae0[i]);
            k = i + 0x81;
            DAT_0067ebc8.a[DAT_0067ebc8.n + 1] = k;
            FUN_0042c300(b, k);
            f = FUN_004282a0(k, 0.7f) + DAT_0067ebc8.h;
            DAT_0067ebc8.h = f + 8.0f;
            DAT_0067ebc8.n += 2;
          }
          break;
        case 6:
          p++;
          if (*p == 6) {
            b = FUN_00423dd0(0);
          }
          DAT_0067ebc8.a[DAT_0067ebc8.n] = 0x81;
          FUN_0042c300(b, 0x81);
          DAT_0067ebc8.h = FUN_004282a0(0x81, 0.7f) + DAT_0067ebc8.h;
          DAT_0067ebc8.a[DAT_0067ebc8.n + 1] = 0xc3;
          DAT_0067ebc8.h = FUN_004282a0(0xc3, 0.7f) + DAT_0067ebc8.h + 16.0f;
          DAT_0067ebc8.n += 2;
          break;
        case 7:
          p++;
          if (*p == 2) {
            b = FUN_00423bc0(0);
            DAT_0067ebc8.a[DAT_0067ebc8.n] = 0x8e;
            FUN_0042c3c0(2000, b, 0x8e, 0);
            DAT_0067ebc8.h = FUN_004282a0(0x8e, 0.7f) + DAT_0067ebc8.h;
          }
          if (*p == 5) {
            b = FUN_00423d50(0);
            DAT_0067ebc8.a[DAT_0067ebc8.n] = 0x8f;
            FUN_0042c3c0(0, b, 0x8f, 1);
            DAT_0067ebc8.h = FUN_004282a0(0x8f, 0.7f) + DAT_0067ebc8.h;
          }
          DAT_0067ebc8.a[DAT_0067ebc8.n + 1] = 0xc3;
          DAT_0067ebc8.h = FUN_004282a0(0xc3, 0.7f) + DAT_0067ebc8.h + 16.0f;
          DAT_0067ebc8.n += 2;
          break;
        case 3:
          p++;
          if (*p == 5 || *p == 0xc) {
            FUN_00424270(DAT_0067eae0, *p, 1);
          }
          else {
            FUN_00424270(DAT_0067eae0, *p, 0);
          }
          n = FUN_0040e340();
          if (DAT_0067ea64 != 0) {
            n = 2;
          }
          for (i = 0; i < n; i++) {
            if (DAT_0067ea64 != 0) {
              v = DAT_007f1a18[DAT_0067eae0[i]].m0;
              FUN_0042c7c0(v);
              DAT_0067ebc8.a[DAT_0067ebc8.n] = v + 0xa4;
              DAT_0067ebc8.h = FUN_004282a0(v + 0xa4, 0.7f) + DAT_0067ebc8.h + 8.0f;
            }
            else {
              v = DAT_007f1a18[DAT_0067eae0[i]].m4 + 0x79;
              DAT_0067ebc8.a[DAT_0067ebc8.n] = v;
              DAT_0067ebc8.h = FUN_004282a0(v, 0.7f) + DAT_0067ebc8.h + 8.0f;
            }
            if (*p == 2) {
              a = FUN_00424100(DAT_0067eae0[i]) * 2000;
              b = FUN_00423bc0(DAT_0067eae0[i]);
              flag = 0;
            }
            if (*p == 3) {
              a = FUN_00423c40(DAT_0067eae0[0]);
              b = FUN_00423c40(DAT_0067eae0[i]);
              flag = 0;
            }
            if (*p == 5) {
              a = FUN_00423d50(DAT_0067eae0[0]);
              b = FUN_00423d50(DAT_0067eae0[i]);
              flag = 1;
            }
            if (*p == 9) {
              a = FUN_004241c0(DAT_0067eae0[i]) * 2000;
              b = FUN_00423e60(DAT_0067eae0[i]);
              flag = 0;
            }
            if (*p == 10) {
              a = FUN_00423ee0(DAT_0067eae0[0]);
              b = FUN_00423ee0(DAT_0067eae0[i]);
              flag = 0;
            }
            if (*p == 0xc) {
              a = FUN_00423ff0(DAT_0067eae0[0]);
              b = FUN_00423ff0(DAT_0067eae0[i]);
              flag = 1;
            }
            k = i + 0x8e;
            DAT_0067ebc8.a[DAT_0067ebc8.n + 1] = k;
            FUN_0042c3c0(a, b, k, flag);
            f = FUN_004282a0(k, 0.7f) + DAT_0067ebc8.h;
            DAT_0067ebc8.h = f + 8.0f;
            DAT_0067ebc8.n += 2;
          }
          break;
        }
      }
    }
    else {
      DAT_0067ebc8.t -= 0xc;
      if ((float)DAT_0067ebc8.t + DAT_0067ebc8.h < 60.0f) {
        FUN_0042c510();
      }
    }
    st = DAT_0067ea6c;
    if (DAT_0067e9fc == 2 && st < 3) {
      DAT_0067ed68 = 0;
      st = 3;
      DAT_0067ea6c = st;
      DAT_0067f0d4 = 0.0f;
      DAT_0067ec24 = 0.0f;
    }
    switch (st) {
    case 0:
      DAT_0067f0d4 = (float)DAT_0067e7cc;
      DAT_0067ed68 = 0x15e;
      FUN_0040b540(DAT_0067eae0);
      DAT_0067ec24 += DAT_007f1004 * 3.0f;
      if (DAT_0067ec24 > 1.0f) {
        DAT_0067ec24 = 1.0f;
        DAT_0067ea6c = 1;
      }
      DAT_0067ed4c = 0;
      break;
    case 1:
      DAT_0067f0d4 = (float)DAT_0067e7cc;
      FUN_0040b540(DAT_0067eae0);
      break;
    case 2:
      FUN_0040b540(DAT_0067eae0);
      DAT_0067f0d4 = 0.0f;
      DAT_0067ec24 = 0.0f;
      if (DAT_0067ed68 >= 20) {
        DAT_0067ed68 -= 20;
      }
      else {
        DAT_0067ed68 = 0;
      }
      if (DAT_0067ed68 == 0) {
        DAT_0067ea6c = 3;
        DAT_0067ebb0 += 40.0f;
        DAT_0067ebb4 += 40.0f;
        DAT_0067ebb8 += 40.0f;
        DAT_0067ebbc += 40.0f;
      }
      break;
    case 3:
      FUN_0040b460(DAT_0067eae0);
      DAT_0067ec24 = 1.0f;
      DAT_0067f0d4 = 255.0f;
      DAT_0067ea6c = 4;
      break;
    case 4:
    case 5:
      FUN_0040b460(DAT_0067eae0);
      break;
    }
  }
  else {
    DAT_0067ec24 -= DAT_007f1004 * 4.0f;
    if (DAT_0067ec24 < 0.0f) {
      DAT_0067ec24 = 0.0f;
    }
  }
  if (DAT_0067ecdc == 1) {
    for (i = 0; i < 4; i++) {
      if (FUN_0040b6c0(i) >= DAT_005f76c4) {
        DAT_0067ed4c = i + 1;
        break;
      }
    }
  }
  if (DAT_0067ecdc == 2) {
    for (i = 0; i < 4; i++) {
      if (FUN_0040b6b0(i) >= DAT_005f76c8) {
        DAT_0067ed4c = i + 1;
        break;
      }
    }
  }
}
