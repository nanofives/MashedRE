// FLAGS: /O2 /GS /Ob1
/* h3 MATCH-T01G-FU 0x00416250 (AI control step, 2015 B): best 88.9% (regs 97.7%, ops 98.5%), 605/605 ins.
   Start was re/match/s03/h4_00416250.c (58.2%). What moved it:
   - float cand[2] declared in a block around the targeting chain (state = 0 .. powerup block): the int->float
     temps ((200 - n), out[4]) then share cand[0]'s slot [esp+0x2c] and the frame offsets all match (+8.7).
   - (float)(...) casts where the original keeps a sub-product: x *= (float)(ang * 0.05f),
     (float)((float)(200 - n) * 0.005f) * amount, the dot product grouped as (float)(a + b) + c. Without them
     cl 9176 reassociates the products / sum.
   - `ang = a = dot` in the state-2 branch gives the original's fst [ang] + register |dot|.
   - literal constants for the 0x005cXXXX floats except the three tail compares, 0.0 (0x005d757c) and 360
     (0x005ccac4 global would also do); DAT_007f0ff4 declared volatile (otherwise the block-2 load is CSEd from
     block 1 and the main paths move out of line).
   - the steer value: the original stores it (fst [esp+0x14]), reads that memory copy in both history blocks and
     keeps the register copy for the four tail tests. Only `volatile float steer` + a register copy
     `s = steer = FUN_00415e20(...)` gives this; non-volatile forms (array/struct with ang, address taken,
     pointer alias, inline helper, block scope, double + (float) cast) are all enregistered or merged.
   - AiCar74.state is at +0x68 (0x0089a52c), not +0x64 as in the s03 file (relocation-masked, relocheck caught it).
   Residual (all class b, source shape not found; the steer part may be class a, 9176 vs the retail build):
   (1) volatile loads its operand first: `fld steer; fcomp b` / `fld steer; fsubr 360` where the original has
   `fld b; fcomp [steer]` / `fld 360; fsub [steer]` (3 sites per history block); (2) EBX/EDI swapped (ours EDI = out,
   EBX = car*0x74 and f; original the reverse), which also flips the first dot-product operand order.
   Tried for (2): a local AiCar74 *c, a local copy of out at several points, operand swaps; permuter
   (--rounds 6 --walk 60, 431 compiles): no gain. relocheck BAD only from the misaligned sites of (1).
   Holds no static callee: 0x004148b0 is a plain cdecl call (4 pushes, add esp 0x10). C++ check: ok. */
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
typedef struct {
  int k;
  float a;
  float b;
  float c;
  int pad;
} SteerHist;
typedef struct {
  char pad0[0x24];
  float e8;
  int side;
  int since;
  int amount;
  char pad1[0x24];
  int age;
  int cool;
  int hold;
  int pad3;
  int state;
  char pad2[0x08];
} AiCar74;
typedef struct {
  int *cur;
  char pad[0xb0];
} CarPwr;
MGLOBALA(SteerHist, DAT_008032d4, 8, 0x008032d4);
MGLOBALA(AiCar74, DAT_0089a4c4, 8, 0x0089a4c4);
MGLOBALA(CarPwr, DAT_0088fc88, 8, 0x0088fc88);
MGLOBAL(volatile int, DAT_007f0ff4, 0x007f0ff4);
MGLOBAL(int, DAT_0089a368, 0x0089a368);
MGLOBAL(int, DAT_007f0ff8, 0x007f0ff8);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005cd09c, 0x005cd09c);
MGLOBAL(float, DAT_005cc72c, 0x005cc72c);
MGLOBAL(float, DAT_005cd0e0, 0x005cd0e0);
int FUN_0040e350(void);
float *FUN_00408af0(int car);
int FUN_0046d4a0(int *p0, int p1);
int FUN_0046d510(float *p0, int p1);
int FUN_0046d6a0(float *p0, int p1);
int FUN_0046d6d0(float *p0, int p1);
float FUN_00443440(int p1, float *pos, float r, float *ang, int z);
void FUN_004161e0(int p1, float *tgt, int car);
int FUN_00414570(int p1, float *tgt, float *out, int car);
int FUN_00415880(int p1, float *tgt, float *out, int car);
int FUN_00414a70(int p1, float *tgt, float *out, int car);
int FUN_00414c30(int p1, float *tgt, float *out, int car);
int FUN_00416060(float *pos, float *out);
int FUN_00415d00(int car);
int FUN_004150e0(float *pos, int car);
int FUN_00443080(void);
int FUN_00426c00(void);
int FUN_00414f00(int p1, float *tgt, float *out, int car);
int FUN_004148b0(int p1, float *tgt, int *out, int car);
int FUN_00415020(int car);
int FUN_00415220(int p1, float *tgt, float *out, int car, int lead);
float FUN_00415e20(int car, float x, float z);
// MATCH: 0x00416250 FUN_00416250
void FUN_00416250(int p1, int car, unsigned char *out)
{
  int mode;
  float speed;
  float h2;
  int rec;
  float pos[2];
  float ang;
  float tgt[2];
  int state;
  volatile float steer;
  float s;
  float delta;
  float x;
  int n;
  float *f;
  float dir[3];
  float a;
  float cross;

  mode = FUN_0040e350();
  FUN_0046d6a0(&speed, car);
  FUN_0046d6d0(&h2, car);
  FUN_0046d4a0(&rec, car);
  pos[0] = *(float *)(rec + 0x30);
  pos[1] = *(float *)(rec + 0x38);
  FUN_00443440(p1, pos, 10.0f, &ang, 0);
  if (ang > 180.0f) {
    ang = 360.0f - ang;
  }
  FUN_004161e0(p1, tgt, car);
  {
    float cand[2];
    int r;
  state = 0;
  if (FUN_00414570(p1, tgt, cand, car) != 0 && FUN_00416060(pos, cand) != 0) {
    tgt[0] = cand[0];
    tgt[1] = cand[1];
    state = 1;
  }
  else if (FUN_00415880(p1, tgt, cand, car) != 0 && FUN_00416060(pos, cand) != 0 &&
           FUN_00415d00(car) == 0) {
    tgt[0] = cand[0];
    tgt[1] = cand[1];
    state = 2;
  }
  else {
    r = FUN_00414a70(p1, tgt, cand, car);
    if (r == 1 && FUN_00416060(pos, cand) != 0) {
      tgt[0] = cand[0];
      tgt[1] = cand[1];
      state = 3;
    }
    else if (r == 2) {
      out[4] = 0;
      out[5] = 0xff;
      return;
    }
    else {
      r = FUN_00414c30(p1, tgt, cand, car);
      if (r == 1 && FUN_00416060(pos, cand) != 0) {
        tgt[0] = cand[0];
        tgt[1] = cand[1];
        state = 3;
      }
      else if (r == 2 && FUN_00416060(pos, cand) != 0) {
        tgt[0] = cand[0];
        tgt[1] = cand[1];
        state = 7;
      }
      else if (FUN_004150e0(pos, car) == 1) {
        state = 9;
      }
    }
  }
  if (FUN_00426c00() == 0x21 && FUN_00414f00(p1, tgt, cand, car) != 0) {
    tgt[0] = cand[0];
    tgt[1] = cand[1];
    state = 10;
  }
  if (mode == 6 && FUN_00443080() == 0) {
    if (state == 0) {
      if (FUN_004148b0(p1, tgt, (int *)cand, car) != 0 && FUN_00416060(pos, cand) != 0) {
        out[5] = 0xff;
        out[0] = 0;
        out[1] = 0;
        return;
      }
      if (FUN_00415020(car) != 0) {
        state = 5;
      }
    }
    if (DAT_0088fc88[car].cur != 0) {
      if (FUN_00415220(p1, tgt, cand, car, (int)ang) != 0 && FUN_00416060(pos, cand) != 0) {
        state = 8;
      }
    }
    else {
      DAT_0089a4c4[car].age = 0;
      DAT_0089a4c4[car].cool = 0;
      DAT_0089a4c4[car].hold = 0;
    }
  }
  }
  DAT_0089a4c4[car].state = state;
  s = steer = FUN_00415e20(car, tgt[0], tgt[1]);
  delta = DAT_005d757c;
  if (steer < 180.0f) {
    DAT_008032d4[car].a = 360.0f;
    if (DAT_008032d4[car].b < steer) {
      delta = DAT_008032d4[car].b;
    }
    DAT_008032d4[car].b = steer;
    if (steer > 1.0f) {
      if (DAT_0089a4c4[car].side != 1 && (n = DAT_007f0ff4 - DAT_0089a4c4[car].since) < 200) {
        out[1] = (unsigned char)(int)((float)((float)(200 - n) * 0.005f) * DAT_0089a4c4[car].amount);
      }
      else {
        x = steer * h2 * 0.003f;
        if (state == 0 && ang > 20.0f) {
          x *= (float)(ang * 0.05f);
        }
        if (x > 255.0f) {
          x = 255.0f;
        }
        out[0] = (unsigned char)(int)x;
        DAT_0089a4c4[car].amount = (int)x;
        DAT_0089a4c4[car].side = 1;
        DAT_0089a4c4[car].since = DAT_007f0ff4;
      }
    }
  }
  if (steer > 180.0f) {
    DAT_008032d4[car].b = 0.0f;
    if (DAT_008032d4[car].a > steer) {
      delta = 360.0f - DAT_008032d4[car].a;
    }
    DAT_008032d4[car].a = steer;
    if (steer < 359.0f) {
      if (DAT_0089a4c4[car].side != 2 && (n = DAT_007f0ff4 - DAT_0089a4c4[car].since) < 200) {
        out[0] = (unsigned char)(int)((float)((float)(200 - n) * 0.005f) * DAT_0089a4c4[car].amount);
      }
      else {
        x = (360.0f - steer) * h2 * 0.003f;
        if (state == 0 && ang > 20.0f) {
          x *= (float)(ang * 0.05f);
        }
        if (x > 255.0f) {
          x = 255.0f;
        }
        out[1] = (unsigned char)(int)x;
        DAT_0089a4c4[car].amount = (int)x;
        DAT_0089a4c4[car].side = 2;
        DAT_0089a4c4[car].since = DAT_007f0ff4;
      }
    }
  }
  out[4] = 0xff;
  a = speed - DAT_008032d4[car].c;
  DAT_008032d4[car].c = speed;
  if (a > 15.0f && speed > 10.0f) {
    out[4] = 0;
    out[5] = 0xff;
  }
  if (delta > 20.0f && h2 > 2000.0f) {
    out[4] = 0;
    out[5] = 0xff;
  }
  if (s < DAT_005cd09c && s > DAT_005cc72c) {
    out[4] = 0xff;
    out[5] = 0xff;
    out[0] = 0xff;
  }
  if (s > DAT_005cd09c && s < DAT_005cd0e0) {
    out[4] = 0xff;
    out[5] = 0xff;
    out[1] = 0xff;
  }
  if (state == 7) {
    out[4] = 0x40;
  }
  else if (state == 5) {
    if (DAT_0089a4c4[car].e8 > 4.0f) {
      out[4] = 0;
    }
    if (DAT_0089a4c4[car].e8 > 6.0f) {
      out[5] = 0x40;
    }
    if ((DAT_007f0ff8 / 60 & 0x20) != 0) {
      out[0] = 0xff;
    }
    else {
      out[1] = 0xff;
    }
  }
  else if (state == 9) {
    if (h2 > 1750.0f) {
      out[4] = 0;
      out[5] = 0xff;
    }
    if (h2 > 1250.0f) {
      out[4] = 0;
    }
  }
  else if (state == 2) {
    f = FUN_00408af0(car);
    FUN_0046d510(dir, car);
    ang = a = (float)(f[1] * dir[1] + dir[0] * f[0]) + dir[2] * f[2];
    cross = dir[0] * f[2] - dir[2] * f[0];
    if (a < DAT_005d757c) {
      a = -a;
    }
    if (cross < DAT_005d757c) {
      if (a < 0.9f) {
        out[1] = 0;
      }
      if (a < 0.8f) {
        out[0] = 0xff;
      }
    }
    else {
      if (a < 0.9f) {
        out[0] = 0;
      }
      if (a < 0.8f) {
        out[1] = 0xff;
      }
    }
  }
  if (DAT_0089a368 == 1) {
    out[4] = (unsigned char)(int)((float)out[4] * 0.4f);
  }
  if (mode != 6 && mode != 5 && mode != 9 && mode != 10 && mode != 11) {
    out[4] = 0;
    out[5] = 0;
  }
}
