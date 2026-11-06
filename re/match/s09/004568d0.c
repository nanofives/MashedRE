// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x004568d0: 76.1% by hand (h5), regs 76.9%, ops 99.6%, 251/250 ins. Raw 24.8%.
   Four 0x48-byte records at 0x006886b0 walked backwards (same array as 0x00456c30), 3 axes + 2 RwRGBA on the stack.
   What moved the score: FUN_00459620 takes 5 args (i, &ltm->pos, 8.0f, 16.0f, colour), Ghidra merged its pushes
   into FUN_004c0ed0's (1 arg). The add esp, 0x14 right after it needs a join point: the call is written twice in
   an if/else on FUN_0045a0f0(i) != -1 (red / blue) and the compiler tail-merges them (67.2% -> 75.6%). i = 4 with
   i-- at the top of a do-while from DAT_006887d0 gives mov edi, 4 / dec edi (76.1%). `r->angX += dX` gives the
   fst/fstp/fld temp spill the original has. Residual: the induction pointer bias. The original addresses every
   field from record+0x2c (esi = 0x006887fc, end test cmp esi, 0x006886dc); 9176 picks +0x24 here. A toy test
   shows 9176 picks the most referenced field (refs in the always-run part weigh more, ties go to the second
   field referenced), so it lands on the angle fields; the original's choice of +0x2c (second field referenced
   in the always-run part) means its angle code references fewer fields through the record than any form tried
   (direct, pointer local, __inline helper: all propagate to the same base). Embedded sub-struct at +0x2c, index
   loop, pointer loop, field-address end test: no change or worse. Permuter (448 variants on the 67.2% form):
   no gain. Every displacement differs by 8 and the end test is lea/cmp instead of one cmp; the rest matches.
   Class (b). */
/* ---- MATCH-S09 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
typedef struct V3_004568d0 { float x, y, z; } V3_004568d0;
typedef struct Rgba004568d0 { unsigned char r, g, b, a; } Rgba004568d0;
typedef struct Mat004568d0 { float m[16]; } Mat004568d0;
typedef struct Atom004568d0 { int m_0; void *parent; } Atom004568d0;
typedef struct Car004568d0 { char pad[0xb0]; int m_b0; } Car004568d0;
typedef struct Rec004568d0 {
  Car004568d0 *car;      /* +0x00 */
  float timer;           /* +0x04 */
  Atom004568d0 *a08;     /* +0x08 */
  int active;            /* +0x0c */
  int m_10;
  int m_14;
  float spin;            /* +0x18 */
  float angZ;            /* +0x1c */
  float angX;            /* +0x20 */
  float angY;            /* +0x24 */
  int m_28;
  Atom004568d0 *a2c;     /* +0x2c */
  Atom004568d0 *a30;     /* +0x30 */
  int m_34;
  Atom004568d0 *a38;     /* +0x38 */
  Atom004568d0 *a3c;     /* +0x3c */
  Atom004568d0 *a40;     /* +0x40 */
  Atom004568d0 *a44;     /* +0x44 */
} Rec004568d0;
MGLOBALA(Rec004568d0, DAT_006886b0, 4, 0x006886b0);
MGLOBAL(int, DAT_006887d0, 0x006887d0);
MGLOBAL(float, DAT_007f100c, 0x007f100c);
void FUN_00474d80(Atom004568d0 *a, int on);
void *FUN_004c1480(void *frame, void *mat, int combine);
int FUN_0045a0f0(int i);
Mat004568d0 *FUN_004c0ed0(void *frame);
void FUN_00459620(int i, V3_004568d0 *pos, float a, float b, Rgba004568d0 *col);
Mat004568d0 *FUN_004c4dc0(Mat004568d0 *out, Mat004568d0 *in);
V3_004568d0 *FUN_0045a110(int k);
V3_004568d0 *FUN_004c3d90(V3_004568d0 *out, V3_004568d0 *in, int n, Mat004568d0 *m);
void *FUN_004c1520(void *frame, V3_004568d0 *axis, float angle, int combine);
void FUN_00476df0(int *p);

// MATCH: 0x004568d0 FUN_004568d0
void FUN_004568d0(void)
{
  V3_004568d0 axisX;
  V3_004568d0 axisY;
  V3_004568d0 axisZ;
  Rgba004568d0 blue;
  Rgba004568d0 red;
  Rec004568d0 *r;
  int i;
  float dX;
  float dY;
  float t;
  V3_004568d0 in;
  V3_004568d0 out;
  Mat004568d0 inv;

  axisX.x = 1.0f; axisX.y = 0.0f; axisX.z = 0.0f;
  axisY.x = 0.0f; axisY.y = 1.0f; axisY.z = 0.0f;
  axisZ.x = 0.0f; axisZ.y = 0.0f; axisZ.z = 1.0f;
  red.r = 0xff; red.g = 0; red.b = 0; red.a = 0xff;
  blue.r = 0; blue.g = 0; blue.b = 0xff; blue.a = 0xff;
  i = 4;
  r = (Rec004568d0 *)&DAT_006887d0;
  do {
    r--;
    i--;
    FUN_00474d80(r->a30, 0);
    FUN_00474d80(r->a2c, 0);
    FUN_00474d80(r->a44, 0);
    if (r->active != 0) {
      FUN_004c1480(r->a08->parent, r->car, 0);
      if (FUN_0045a0f0(i) != -1) {
        FUN_00459620(i, (V3_004568d0 *)((char *)FUN_004c0ed0(r->a44->parent) + 0x30), 8.0f, 16.0f, &red);
      } else {
        FUN_00459620(i, (V3_004568d0 *)((char *)FUN_004c0ed0(r->a44->parent) + 0x30), 8.0f, 16.0f, &blue);
      }
      dX = 0.0f;
      dY = 0.0f;
      FUN_004c4dc0(&inv, FUN_004c0ed0(r->a40->parent));
      in = *FUN_0045a110(r->car->m_b0);
      FUN_004c3d90(&out, &in, 1, &inv);
      if (out.x > 0.1f || out.x < -0.1f) {
        dY = out.x * 2.5f;
      }
      if (out.y > 0.1f || out.y < -0.1f) {
        dX = out.y * -2.5f;
        if (dX > 3.0f) {
          dX = 3.0f;
        } else if (dX < -3.0f) {
          dX = -3.0f;
        }
      }
      if (dY > 3.0f) {
        dY = 3.0f;
      } else if (dY < -3.0f) {
        dY = -3.0f;
      }
      r->angX += dX;
      if (r->angX > 360.0f) {
        r->angX -= 360.0f;
      }
      if (r->angX < 0.0f) {
        r->angX += 360.0f;
      }
      r->angY += dY;
      if (r->angY > 360.0f) {
        r->angY -= 360.0f;
      }
      if (r->angY < 0.0f) {
        r->angY += 360.0f;
      }
      FUN_004c1520(r->a38->parent, &axisY, r->angY, 0);
      FUN_004c1520(r->a40->parent, &axisX, r->angX, 0);
      FUN_004c1520(r->a3c->parent, &axisZ, r->angZ, 0);
      r->spin = r->spin - 1.0f;
      if (r->spin < 0.0f) {
        r->spin = 0.0f;
      }
      r->angZ = r->angZ + r->spin;
      if (r->timer > 0.0f) {
        r->timer = r->timer - DAT_007f100c;
      } else {
        r->timer = 0.0f;
      }
    }
  } while (r != DAT_006886b0);
  FUN_00476df0(&DAT_006887d0);
}
