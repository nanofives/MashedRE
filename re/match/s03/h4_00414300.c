// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00414300 FUN_00414300
// MATCH: 0x00414a70 FUN_00414a70
/* h4 MATCH-S03. 0x00414a70 (447 B): 100.0%, BYTE-IDENTICAL (--bytes, relocations masked).
   0x00414300 (398 B): 97.8% (regs 97.8, ops 100), 134/134 ins, 4 differing bytes at +0xf2.
   g2s start: 0x00414300 67.9%, 0x00414a70 49.0%. Ordinary cdecl pair, no register-passed args.
   0x00414300 steps: _DAT_005cc320 (1.0) / _DAT_005cc558 are floats, not ints; the two slope tests are
   `!= 0` with the general case first (Ghidra inverted both) -> 83.6 -> 90.3%. The facing test reads a local
   vector v[3] (v[0] = pt.x - a.x, v[2] = pt.y - a.y) rather than an inline expression or two scalars:
   only the array form keeps both differences on the x87 stack -> 97.8%.
   Residual (0x004143ef..0x004143fb): original computes dir[0]*v[0] first (fld [esp+8]; fmul st(2)) then
   dir[2]*v[2]; ours computes dir[2]*v[2] first. Tried, none moves it: all term orders and operand orders
   (5), a 3-term dot with v[1] = 0 and dir[1] = 0 (folds to the same), dot into a named temp, `0 < dot`,
   `!(dot <= 0)`, double terms, V3 struct instead of arrays, a static __inline Dot3 helper (both arg
   orders), moving dir[1] = 0 before/after v, v[2] assigned before v[0] (worse, 95.5%); permuter 183
   compiles, no gain. Class (a) candidate: x87 evaluation order of two commutative, equal-cost products
   that no statement/term order changes.
   0x00414a70 notes: the progress value of the scanned car and the out-float of the second FUN_0046d6d0
   call are ONE variable (`prog`, stack slot 0x14): that single merge took 79.1% -> 100%. */
#ifndef MATCH_PORT
extern float DAT_005d757c;
extern float DAT_005cc320;
extern float DAT_005cc558;
int FUN_0046d510(float *p0, int p1);
float FUN_004c3b30(float p0);
extern float DAT_005cd0ac;
extern float DAT_005cc348;
float FUN_00408a50(int p0);
int FUN_0040e370(int p0);
int FUN_0040e470(int p0);
int FUN_00426bb0(void);
int FUN_0046d4a0(int *p0, int p1);
int FUN_0046d6d0(float *p0, int p1);
float FUN_004c3ac0(float *p0);
#endif
int FUN_00414300(int car, float *a, float *b, float *pt, float radius, float *out)
{
  float x;
  float y;
  float m;
  float c;
  float p;
  float dx;
  float dy;
  float d;
  float inv;
  float dir[3];
  float v[3];

  if (a[0] - b[0] != DAT_005d757c) {
    m = (a[1] - b[1]) / (a[0] - b[0]);
    if (m != DAT_005d757c) {
      p = -(DAT_005cc320 / m);
      c = a[1] - m * a[0];
      x = ((pt[1] - p * pt[0]) - c) / (m - p);
      y = m * x + c;
    }
    else {
      x = pt[0];
      y = a[1];
    }
  }
  else {
    x = a[0];
    y = pt[1];
  }
  dx = pt[0] - x;
  dy = pt[1] - y;
  d = dy * dy + dx * dx;
  if (d < radius * radius) {
    FUN_0046d510(dir, car);
    dir[1] = 0.0f;
    v[0] = pt[0] - a[0];
    v[2] = pt[1] - a[1];
    if (dir[0] * v[0] + dir[2] * v[2] > DAT_005d757c) {
      if (d > DAT_005cc558) {
        inv = DAT_005cc320 / FUN_004c3b30(d);
        out[0] = pt[0] - dx * inv * radius;
        out[1] = pt[1] - inv * dy * radius;
        return 1;
      }
      out[0] = pt[0] - dx * radius;
      out[1] = pt[1] - dy * radius;
      return 1;
    }
  }
  return 0;
}

int FUN_00414a70(int unused, float *p2, float *out, int car)
{
  int rec;
  float pos[2];
  float my_h;
  float my_prog;
  float dir[3];
  int best;
  float best_d;
  int i;
  float prog;
  float diff;
  float other[2];
  float v[3];
  float dist;

  FUN_0046d4a0(&rec, car);
  pos[0] = *(float *)(rec + 0x30);
  pos[1] = *(float *)(rec + 0x38);
  FUN_0046d6d0(&my_h, car);
  FUN_0046d510(dir, car);
  dir[1] = 0.0f;
  my_prog = FUN_00408a50(car);
  best = -1;
  best_d = 2.0f;
  for (i = 0; i < 4; i++) {
    if (i != car && FUN_0040e370(i) != 0) {
      prog = FUN_00408a50(i);
      diff = prog - my_prog;
      if (diff > DAT_005d757c || (float)(5 - FUN_00426bb0()) > diff) {
        if (diff < best_d) {
          best_d = diff;
          best = i;
        }
      }
    }
  }
  if (best != -1) {
    FUN_0046d4a0(&rec, best);
    other[0] = *(float *)(rec + 0x30);
    other[1] = *(float *)(rec + 0x38);
    v[0] = other[0] - pos[0];
    v[1] = 0.0f;
    v[2] = other[1] - pos[1];
    dist = FUN_004c3ac0(v);
    FUN_0046d6d0(&prog, best);
    if (my_h - prog > DAT_005cd0ac || my_h < DAT_005cd0ac) {
      if (FUN_00414300(car, pos, p2, other, 1.0f, out) != 0) {
        return 1;
      }
    }
    if (FUN_0040e470(best) != 1 && dist < DAT_005cc348) {
      return 2;
    }
  }
  return 0;
}
