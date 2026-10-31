// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00417da0 FUN_00417da0
/* h4 MATCH-S03 round 2. 0x00417da0 (1980 B): 66.5% (regs 74.7, ops 92.1), 594/597 ins. g2s did not
   compile (C2069). Same body as 0x00416250 except FUN_00417cf0 in place of FUN_004148b0 and no
   DAT_0089a368 tail. Adds over 0x00416a30: FUN_00426c00()==0x21 -> FUN_00414f00 (state 10), state==0 ->
   FUN_00417cf0 / FUN_00415020 (state 5), car[].state (+0x68, 0x0089a52c) = state.
   Residual: the same steer memory-home issue as 0x00416a30 (stack offsets shifted by 4), plus here cl keeps
   state in EBX and uses BL for the two zero bytes, as the original does. Goto form of the chain: 59.1
   (kept the nested form). Class (b) unknown. */
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
  int state;
  char pad2[0x0c];
} AiCar74;
typedef struct {
  int *cur;
  char pad[0xb0];
} CarPwr;
#ifndef MATCH_PORT
extern SteerHist DAT_008032d4[];
extern AiCar74 DAT_0089a4c4[];
extern CarPwr DAT_0088fc88[];
extern int DAT_007f0ff4;
extern int DAT_007f0ff8;
extern float DAT_005d757c;
extern float DAT_005cd09c;
extern float DAT_005ccac4;
extern float DAT_005cc320;
extern float DAT_005cd0e4;
extern float DAT_005cd0e8;
extern float DAT_005cd0ec;
extern float DAT_005cd04c;
extern float DAT_005ccd6c;
extern float DAT_005cc9a0;
extern float DAT_005cc9b0;
extern float DAT_005cc55c;
extern float DAT_005cd0b8;
extern float DAT_005cc72c;
extern float DAT_005cd0e0;
extern float DAT_005cc35c;
extern float DAT_005cd0a0;
extern float DAT_005cd0dc;
extern float DAT_005cd0d8;
extern float DAT_005cc9c8;
extern float DAT_005cc9bc;
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
int FUN_00417cf0(int p1, float *tgt, float *out, int car);
int FUN_00415020(int car);
int FUN_00415220(int p1, float *tgt, float *out, int car, int lead);
float FUN_00415e20(int car, float x, float z);
#endif
void FUN_00417da0(int p1, int car, unsigned char *out)
{
  int mode;
  float speed;
  float h2;
  int rec;
  float pos[2];
  float ang;
  float tgt[2];
  float cand[2];
  int state;
  int r;
  float steer;
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
  if (ang > DAT_005cd09c) {
    ang = DAT_005ccac4 - ang;
  }
  FUN_004161e0(p1, tgt, car);
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
      if (FUN_00417cf0(p1, tgt, cand, car) != 0 && FUN_00416060(pos, cand) != 0) {
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
  DAT_0089a4c4[car].state = state;
  steer = FUN_00415e20(car, tgt[0], tgt[1]);
  delta = DAT_005d757c;
  if (steer < DAT_005cd09c) {
    DAT_008032d4[car].a = 360.0f;
    if (DAT_008032d4[car].b < steer) {
      delta = DAT_008032d4[car].b;
    }
    DAT_008032d4[car].b = steer;
    if (steer > DAT_005cc320) {
      if (DAT_0089a4c4[car].side == 1 || (n = DAT_007f0ff4 - DAT_0089a4c4[car].since) >= 200) {
        x = steer * h2 * DAT_005cd0e8;
        if (state == 0 && ang > DAT_005ccd6c) {
          x *= ang * DAT_005cc9a0;
        }
        if (x > DAT_005cd04c) {
          x = DAT_005cd04c;
        }
        out[0] = (unsigned char)(int)x;
        DAT_0089a4c4[car].amount = (int)x;
        DAT_0089a4c4[car].side = 1;
        DAT_0089a4c4[car].since = DAT_007f0ff4;
      }
      else {
        out[1] = (unsigned char)(int)((float)(200 - n) * DAT_005cd0ec * DAT_0089a4c4[car].amount);
      }
    }
  }
  if (steer > DAT_005cd09c) {
    DAT_008032d4[car].b = 0.0f;
    if (DAT_008032d4[car].a > steer) {
      delta = DAT_005ccac4 - DAT_008032d4[car].a;
    }
    DAT_008032d4[car].a = steer;
    if (steer < DAT_005cd0e4) {
      if (DAT_0089a4c4[car].side == 2 || (n = DAT_007f0ff4 - DAT_0089a4c4[car].since) >= 200) {
        x = (DAT_005ccac4 - steer) * h2 * DAT_005cd0e8;
        if (state == 0 && ang > DAT_005ccd6c) {
          x *= ang * DAT_005cc9a0;
        }
        if (x > DAT_005cd04c) {
          x = DAT_005cd04c;
        }
        out[1] = (unsigned char)(int)x;
        DAT_0089a4c4[car].amount = (int)x;
        DAT_0089a4c4[car].side = 2;
        DAT_0089a4c4[car].since = DAT_007f0ff4;
      }
      else {
        out[0] = (unsigned char)(int)((float)(200 - n) * DAT_005cd0ec * DAT_0089a4c4[car].amount);
      }
    }
  }
  out[4] = 0xff;
  a = speed - DAT_008032d4[car].c;
  DAT_008032d4[car].c = speed;
  if (a > DAT_005cc9b0 && speed > DAT_005cc55c) {
    out[4] = 0;
    out[5] = 0xff;
  }
  if (delta > DAT_005ccd6c && h2 > DAT_005cd0b8) {
    out[4] = 0;
    out[5] = 0xff;
  }
  if (steer < DAT_005cd09c && steer > DAT_005cc72c) {
    out[4] = 0xff;
    out[5] = 0xff;
    out[0] = 0xff;
  }
  if (steer > DAT_005cd09c && steer < DAT_005cd0e0) {
    out[4] = 0xff;
    out[5] = 0xff;
    out[1] = 0xff;
  }
  if (state == 7) {
    out[4] = 0x40;
  }
  else if (state == 5) {
    if (DAT_0089a4c4[car].e8 > DAT_005cc35c) {
      out[4] = 0;
    }
    if (DAT_0089a4c4[car].e8 > DAT_005cd0a0) {
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
    if (h2 > DAT_005cd0dc) {
      out[4] = 0;
      out[5] = 0xff;
    }
    if (h2 > DAT_005cd0d8) {
      out[4] = 0;
    }
  }
  else if (state == 2) {
    f = FUN_00408af0(car);
    FUN_0046d510(dir, car);
    ang = f[1] * dir[1] + dir[0] * f[0] + dir[2] * f[2];
    a = ang;
    if (a < DAT_005d757c) {
      a = -a;
    }
    cross = dir[0] * f[2] - dir[2] * f[0];
    if (cross < DAT_005d757c) {
      if (a < DAT_005cc9c8) {
        out[1] = 0;
      }
      if (a < DAT_005cc9bc) {
        out[0] = 0xff;
      }
    }
    else {
      if (a < DAT_005cc9c8) {
        out[0] = 0;
      }
      if (a < DAT_005cc9bc) {
        out[1] = 0xff;
      }
    }
  }
  if (mode != 6 && mode != 5 && mode != 9 && mode != 10 && mode != 11) {
    out[4] = 0;
    out[5] = 0;
  }
}
