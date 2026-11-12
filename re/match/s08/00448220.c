// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00448220: 97.0% (regs 97.0%) by hand (h8), raw 22.6%. Ghidra's body (2184 B) runs into the next
   function: the real one is 1248 B and ends in a tail jump to 0x00434360 (written as the last call). What moved it:
   V3 locals passed by value (FUN_0046d400, FUN_00442e00), FUN_00408a50/FUN_00472650/FUN_0046d400/FUN_004c3ac0
   returning float, one float local reused for the half-width, the lerp complement and the scale (gives the
   original slot order), (v4.z + v6.z) operand order (puts v4 above v6 in the frame), the second
   FUN_00409330 result held in a variable (keeps the deferred add esp), and 0x005cc320/0x005cc950/0x005ce1ec read as
   const globals in the scale (with literals the compiler folds 1.0f/len*0.75f into 0.75f/len).
   Residual (b): the last block (scale b, store DAT_00898064/0089804c/00898050, a = b + pos) schedules the
   fld/fmul around the arg pushes differently; every statement order of that block and the permuter
   (538 compiles) give the same 97.0%. */
/* ---- MATCH-S08 preamble (same text in every re/match/s08 source; not part of any function body) ----
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
typedef struct { float x; float y; float z; } V3_00448220;
MGLOBAL(const float, DAT_005cc320, 0x005cc320);
MGLOBAL(const float, DAT_005cc950, 0x005cc950);
MGLOBAL(const float, DAT_005ce1ec, 0x005ce1ec);
MGLOBAL(int, DAT_00898064, 0x00898064);
MGLOBAL(float, DAT_0089804c, 0x0089804c);
MGLOBAL(float, DAT_00898050, 0x00898050);
MGLOBAL(int, DAT_007f1a50, 0x007f1a50);
MGLOBALA(char, DAT_00897fe0, 4, 0x00897fe0);
MGLOBAL(int, DAT_00896498, 0x00896498);
MGLOBAL(int, DAT_00898988, 0x00898988);
MGLOBAL(int, DAT_00898990, 0x00898990);
MGLOBALA(char, DAT_00803324, 4, 0x00803324);
void FUN_00441990(char *cam);
void FUN_004c1c10(int p0, int p1);
int FUN_0040e350(void);
int FUN_0040e370(int i);
int FUN_0046c7b0(int i);
void FUN_0046cbb0(int i, int *out, char *buf);
int FUN_0040e4b0(void);
void FUN_0046d4a0(int *out, int idx);
void FUN_0046b4f0(V3_00448220 *out, int idx, int which);
void FUN_0046d510(V3_00448220 *out, int idx);
int FUN_0040e4a0(void);
float FUN_00472650(int p0, float p1);
void FUN_00446520(char *cam, int p1);
float FUN_00408a50(int idx);
int FUN_00409330(int h, int p1);
void FUN_00408b00(int h, int p1, int p2, V3_00448220 *out, unsigned char *col);
float FUN_0046d400(V3_00448220 v, char *p);
void FUN_0046cc10(int *out, int idx);
float FUN_004c3ac0(V3_00448220 *v);
void FUN_00442e00(char *cam, V3_00448220 from, V3_00448220 to, float p3, float p4);
void FUN_0041ef60(int i, int p1);
void FUN_00434360(void);
// MATCH: 0x00448220 FUN_00448220
void FUN_00448220(int param_1, int param_2, int param_3)
{
  float f;
  int n;
  int out18;
  int rec;
  unsigned char col[4];
  int out24;
  char buf28[4];
  V3_00448220 a;
  V3_00448220 b;
  V3_00448220 pos;
  V3_00448220 dir;
  V3_00448220 v6;
  V3_00448220 v4;
  int mode;
  int cnt;
  int i;
  int idx;
  int h;
  int flag;
  float frac;
  float g;
  float scale;
  int t;

  DAT_00898064 = param_1;
  DAT_0089804c = 1.0f;
  DAT_00898050 = 1.0f;
  if (DAT_007f1a50 == 1) {
    FUN_00441990(DAT_00897fe0);
    return;
  }
  FUN_004c1c10(param_1, 1);
  mode = FUN_0040e350();
  if (mode == 6) {
    cnt = 0;
    for (i = 0; i < 4; i++) {
      if (FUN_0040e370(i) != 0 && FUN_0046c7b0(i) == 1) {
        FUN_0046cbb0(i, &out18, buf28);
        if (out18 == 0) {
          cnt++;
        }
      }
    }
    if (cnt != 0) {
      goto reset;
    }
    mode = -1;
  } else if (mode != 7 && mode != -1) {
reset:
    DAT_00896498 = 0;
    DAT_00898988 = 0;
    DAT_00898990 = 0;
    FUN_00446520(DAT_00897fe0, param_3);
    return;
  }
  idx = FUN_0040e4b0();
  if (idx == -1) {
    idx = 0;
  }
  FUN_0046d4a0(&rec, idx);
  pos.x = *(float *)(rec + 0x30);
  pos.y = *(float *)(rec + 0x34) + 0.15f;
  pos.z = *(float *)(rec + 0x38);
  FUN_0046b4f0(&v4, idx, 4);
  FUN_0046b4f0(&v6, idx, 6);
  f = (v4.z + v6.z) * 0.5f;
  FUN_0046d510(&dir, idx);
  pos.x = dir.x  * f + pos.x;
  pos.z = dir.z  * f + pos.z;
  if (FUN_0040e4a0() < 5) {
    DAT_00898988 = 1;
    DAT_00898990 = (int)FUN_00472650(0, 10.0f);
  }
  DAT_00898988 = DAT_00898988 + 1;
  t = DAT_00898988 & 0x1f;
  if (t > 15) {
    t = 31 - t;
  }
  t = t * 12;
  if (DAT_00898988 > 0x20) {
    t = DAT_00898988 - 0x20;
    if (t > 15) {
      t = 31 - t;
    }
    t = t * 3;
  }
  if (DAT_00898988 > 0x40) {
    t = 0;
  }
  DAT_00896498 = (DAT_00898990 * t) / 10;
  if (DAT_00896498 < 0) {
    DAT_00896498 = 0;
  }
  frac = FUN_00408a50(idx);
  n = (int)frac;
  frac = frac - (float)n;
  h = FUN_00409330(n, 0);
  FUN_00408b00(h, 0, 1, &a, col);
  h = FUN_00409330(h, 0);
  FUN_00408b00(h, 0, 1, &b, col);
  f = 1.0f - frac;
  a.x = b.x * frac + a.x * f;
  a.y = b.y * frac + a.y * f;
  a.z = b.z * frac + a.z * f;
  g = FUN_0046d400(a, DAT_00803324);
  flag = 0;
  a.y = (a.y - g) + 0.5f;
  if (mode == -1) {
    a.y = a.y + 8.0f;
  } else {
    if (FUN_0040e4b0() >= 0 && FUN_0040e4b0() < 4) {
      FUN_0046cc10(&out24, FUN_0040e4b0());
      if (out24 == 4) {
        goto skip;
      }
    }
    flag = 1;
    a.y = a.y + 10.0f;
  }
skip:
  b.x = a.x - pos.x;
  b.y = a.y - pos.y;
  b.z = a.z - pos.z;
  f = DAT_005ce1ec / FUN_004c3ac0(&b) * DAT_005cc950 * DAT_005cc320;
  if (mode == -1 || flag != 0) {
    f = 1.0f;
  }
  n = FUN_0040e4a0();
  if (n > 400) {
    n = 400;
  }
  a.y = a.y - (float)n * 0.00066666666f;
  b.x = a.x - pos.x;
  b.y = a.y - pos.y;
  b.z = a.z - pos.z;
  FUN_004c3ac0(&b);
  b.x = b.x * f;
  b.y = b.y * f;
  b.z = b.z * f;
  DAT_00898064 = param_1;
  DAT_0089804c = 1.0f;
  DAT_00898050 = 1.0f;
  a.x = b.x + pos.x;
  a.y = b.y + pos.y;
  a.z = b.z + pos.z;
  FUN_00442e00(DAT_00897fe0, pos, a, 1.0f, 0.2f);
  FUN_0040e4a0();
  for (i = 0; i < 4; i++) {
    FUN_0041ef60(i, 0);
  }
  FUN_00434360();
}
