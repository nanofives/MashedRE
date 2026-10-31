// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00415220 FUN_00415220
/* h4 MATCH-S03 round 2. 0x00415220 (1547 B): 79.4% (regs 79.4, ops 90.3), 430/432 ins. g2s start 35.0%.
   Real prototype (call site 0x00416c89, AiControlStep.cpp:72): (p1, tgt, out, car, int lead), cdecl; lead is
   (int)ang from the caller. No register args: Ghidra's in_stack_* came from the FUN_0046d6d0 out-float that cl
   places in car's own dead parameter slot (+0x10), which plain C reproduces.
   Shape: switch on *DAT_0088fc88[car].cur, cases 7/9/11/17/10/12/16/18/19 in that source order (it is the
   emitted order); per-car 0x74 records indexed directly (a hoisted pointer gives `add esi,base`, 66.2%);
   `!(r < K)` for every random test (test ah,5 / jp), `<=` for the early-outs (test ah,0x41).
   Residuals: (1) cross-jumping. The original shares some identical tails (cool=3000 / 9000 / 6000,
   fire+cool=1500/500, armed=1) and keeps others inline (case 9 armed=1, case 11 fire). cl 9176 merges a
   different set: case 9's tail into case 16's, case 18 into 19. Tried inverting case 16's test
   (78.7), operand order of the case 7 compare (`>=`, swapped sides, a temp, int cast; best 79.4 with the
   temp). (2) the case-17 dot product: original f[1]*n[1] first, ours another order; 5 term orders and
   local decl orders do not move it (same as 0x00414300). Permuter: 85 compiles, no gain.
   Class (b) unknown for the tail sharing (source shape of the shared tails not found), (a) candidate for
   the dot-product order. */
typedef struct {
  char pad0[5];
  char vis;
  char pad1;
  char fire;
  char pad2[0x44];
} PwrSlot;
typedef struct {
  int slot;
  int pad[3];
} CarSlotIdx;
typedef struct {
  int *cur;
  char pad[0xb0];
} CarPwr;
typedef struct {
  char pad0[0x58];
  int age;
  int cool;
  int hold;
  int armed;
  char pad1[0x0c];
} AiPwr;
#ifndef MATCH_PORT
extern PwrSlot DAT_007f1038[];
extern CarSlotIdx DAT_007f1a14[];
extern CarPwr DAT_0088fc88[];
extern AiPwr DAT_0089a4c4[];
extern int DAT_007f1008;
extern float DAT_0089a360;
extern float DAT_005cc730;
extern float DAT_005cc32c;
extern float DAT_005cd0b0;
extern float DAT_005cc950;
extern float DAT_005cc9c8;
extern float DAT_005cd0b8;
extern float DAT_005cc9b0;
extern float DAT_005cd0c0;
extern float DAT_005cc9e0;
extern float DAT_005cc55c;
extern float DAT_005cd0bc;
extern float DAT_005cc320;
float *FUN_00408af0(int car);
int FUN_00415190(int car, float lo, float hi);
int FUN_00415200(void);
float FUN_00442cc0(int car);
int FUN_00455b40(int car);
int FUN_0045a0f0(int car);
int FUN_0046d4a0(int *p0, int p1);
int FUN_0046d6d0(float *p0, int p1);
float FUN_00472650(float lo, float hi);
float FUN_004c39b0(float *out, float *v);
float FUN_004c3ac0(float *v);
#endif
int FUN_00415220(void *p1, void *p2, void *p3, int car, int lead)
{
  PwrSlot *slot;
  float h;
  int t;
  int r0;
  int r1;
  float len;
  float v[3];
  float n[3];
  float *f;
  float d;

  slot = &DAT_007f1038[DAT_007f1a14[car].slot];
  FUN_0046d6d0(&h, car);
  t = DAT_0089a4c4[car].age + DAT_007f1008;
  DAT_0089a4c4[car].age = t;
  if (t > 60000) {
    DAT_0089a4c4[car].hold = 1;
  }
  if (t < 3000) {
    DAT_0089a4c4[car].cool = 3000;
  }
  if (DAT_0089a4c4[car].cool != 0) {
    DAT_0089a4c4[car].cool -= DAT_007f1008;
    if (DAT_0089a4c4[car].cool < 0) {
      DAT_0089a4c4[car].cool = 0;
    }
  }
  if (DAT_0088fc88[car].cur == 0) {
    return 0;
  }
  switch (*DAT_0088fc88[car].cur) {
  case 7:
    if (DAT_0089a4c4[car].hold == 0) {
      if (DAT_0089a4c4[car].cool != 0) return 0;
      if (FUN_00415200() == 0) return 0;
      if (FUN_00442cc0(car) <= DAT_005cd0c0) return 0;
      if (!((float)lead < DAT_005cc9e0)) return 0;
      d = FUN_00472650(0.0f, 1.0f);
      if (!(d < DAT_005cc320 - (DAT_005cc55c - (float)(int)DAT_0089a360) * DAT_005cd0bc)) {
        DAT_0089a4c4[car].cool = 3000;
        return 0;
      }
    }
    slot->fire = 1;
    DAT_0089a4c4[car].cool = 750;
    return 0;
  case 9:
    if (DAT_0089a4c4[car].armed != 0) {
      slot->fire = 1;
      if (FUN_0045a0f0(car) != 0) {
        DAT_0089a4c4[car].armed = 0;
      }
      return 0;
    }
    if (DAT_0089a4c4[car].hold != 0) {
      DAT_0089a4c4[car].armed = 1;
      return 0;
    }
    if (DAT_0089a4c4[car].cool != 0) return 0;
    if (FUN_00415200() == 0) return 0;
    if (FUN_0045a0f0(car) != 0) return 0;
    if (FUN_00472650(0.0f, 1.0f) < DAT_005cc950) {
      DAT_0089a4c4[car].armed = 1;
      return 0;
    }
    DAT_0089a4c4[car].cool = 3000;
    return 0;
  case 11:
    if (DAT_0089a4c4[car].hold != 0) {
      slot->fire = 1;
    }
    if (DAT_0089a4c4[car].cool != 0) return 0;
    if (FUN_00415200() == 0) return 0;
    if (FUN_0045a0f0(car) != 0) return 0;
    if (FUN_00455b40(car) == 0) return 0;
    if (FUN_00472650(0.0f, 1.0f) < DAT_005cc950) {
      slot->fire = 1;
      return 0;
    }
    DAT_0089a4c4[car].cool = 6000;
    return 0;
  case 17:
    if (DAT_0089a4c4[car].hold == 0) {
      if (DAT_0089a4c4[car].cool != 0) return 0;
      if (FUN_00415190(car, 0.5f, 3.0f) == 0) return 0;
      FUN_0046d4a0(&r0, 0);
      FUN_0046d4a0(&r1, car);
      v[0] = *(float *)(r0 + 0x30) - *(float *)(r1 + 0x30);
      v[1] = 0.0f;
      v[2] = *(float *)(r0 + 0x38) - *(float *)(r1 + 0x38);
      len = FUN_004c3ac0(v);
      FUN_004c39b0(n, v);
      f = FUN_00408af0(car);
      d = -((f[1] * n[1] + n[0] * f[0] + f[2] * n[2]) * len);
      if (!(d > DAT_005cd0b0 && d < DAT_005cc950)) return 0;
      if (!(FUN_00472650(0.0f, 1.0f) < DAT_005cc9c8)) {
        DAT_0089a4c4[car].cool = 3000;
        return 0;
      }
    }
    slot->fire = 1;
    DAT_0089a4c4[car].cool = 500;
    return 0;
  case 10:
    if (DAT_0089a4c4[car].hold == 0) {
      if (DAT_0089a4c4[car].cool != 0) return 0;
      if (h <= DAT_005cd0b8) return 0;
      if (FUN_00415190(car, 4.0f, 10.0f) == 0) return 0;
      if (!((float)lead < DAT_005cc9b0)) return 0;
      if (!(FUN_00472650(0.0f, 1.0f) < DAT_005cc9c8)) {
        DAT_0089a4c4[car].cool = 6000;
        return 0;
      }
    }
    slot->fire = 1;
    DAT_0089a4c4[car].cool = 1500;
    return 0;
  case 12:
    if (DAT_0089a4c4[car].hold == 0) {
      if (DAT_0089a4c4[car].cool != 0) return 0;
      if (h <= DAT_005cd0b8) return 0;
      if (FUN_00415190(car, 4.0f, 10.0f) == 0) return 0;
      if (!(FUN_00472650(0.0f, 1.0f) < DAT_005cc950)) {
        DAT_0089a4c4[car].cool = 9000;
        return 0;
      }
    }
    slot->fire = 1;
    DAT_0089a4c4[car].cool = 1500;
    return 0;
  case 16:
    if (DAT_0089a4c4[car].armed != 0) {
      slot->fire = 1;
      return 0;
    }
    if (DAT_0089a4c4[car].hold != 0) {
      DAT_0089a4c4[car].armed = 1;
      return 0;
    }
    if (DAT_0089a4c4[car].cool != 0) return 0;
    if (h <= DAT_005cd0b8) return 0;
    if (FUN_00415190(car, 3.0f, 10.0f) == 0) return 0;
    if (!(FUN_00472650(0.0f, 1.0f) < DAT_005cc32c)) {
      DAT_0089a4c4[car].cool = 3000;
      return 0;
    }
    DAT_0089a4c4[car].armed = 1;
    return 0;
  case 18:
    if (DAT_0089a4c4[car].hold != 0) {
      slot->fire = 1;
    }
    if (DAT_0089a4c4[car].cool != 0) return 0;
    if ((float)lead <= DAT_005cc730) return 0;
    if (!(FUN_00472650(0.0f, 1.0f) < DAT_005cc32c)) {
      DAT_0089a4c4[car].cool = 9000;
      return 0;
    }
    slot->fire = 1;
    return 0;
  case 19:
    if (DAT_0089a4c4[car].armed != 0) {
      slot->fire = 1;
      return 0;
    }
    if (DAT_0089a4c4[car].hold != 0) {
      DAT_0089a4c4[car].armed = 1;
      return 0;
    }
    if (DAT_0089a4c4[car].cool != 0) return 0;
    if ((float)lead <= DAT_005cc730) return 0;
    if (!(FUN_00472650(0.0f, 1.0f) < DAT_005cc32c)) {
      DAT_0089a4c4[car].cool = 9000;
      return 0;
    }
    DAT_0089a4c4[car].armed = 1;
    return 0;
  }
  return 0;
}
