// FLAGS: /O2 /GS /Ob1
/* MATCH-RACEPATH-SHIFT 2026-11-09: 88.1% / 256 bad B -> 93.4% / 51 bad B, symbol 487 -> 495 B (the original's
   length; first divergence stays at +0x3e). (1) Dot terms written so the operand order follows the original:
   `(0, m->up.x) * v.x` (field first), block 2 `v.x*zx + (0, v.y*zy) + v.z*zz` (the comma stops 9176 factoring
   (v.x+v.y)*0, which was the length change), block 3 `(0, x-term + y-term) + (0, m->right.z) * v.z`. (2) `d = acos(..);
   d = d * 57.29578f;` as two statements puts the compare load before `add esp, 8` in all three blocks, as in
   the original. (3) The a[0]/a[1] stores moved below the next block's first matrix copy (permuter, independent
   stores) and the v.z copy before a[0]/a[1]. relocheck OK. Left: residual (1) below (load/store scheduling of
   the v copies around the two pushes of FUN_004c39b0), 12 instruction rows. The text below is the earlier state. */
/* a3 MATCH-S01: 0x00405540 (495 B) best 88.1% (g2s did not compile). Debug roll/pitch/yaw of the
   0x00639d78 object frame (RwFrame modelling matrix at +0x10), prints twice via printf (0x004a2cbd).
   Found: acos is the CRT stack-arg entry 0x004a3384 (declared as FUN_004a3384 to avoid the _CIacos intrinsic); 0x005cc970 is a double literal holding
   (double)57.29578f; 0x005d757c is the 0.0f literal; each angle is a double temp negated on the FPU stack
   (`if (!(v.y < 0.0f))` / `!(v.z > 0.0f)` give the jnp / je forms); out is walked with `*++out` and a lone
   `out++` (85.4%); a local {0,0,1} axis for the middle dot moved the compare loads before add esp (88.1%).
   Residual (b): (1) in each block the zero store to v and the second matrix-element load are scheduled after
   the two pushes in the original; (2) dot-product operand order (fld m; fmul v on the zeroed term) and the
   middle dot, which 9176 factors as (v.y + v.x) * 0.0f while the original keeps v.y*0 + v.x*0. Term order,
   macro vs __inline helper, pointer locals, struct-copy-then-zero, store orders: none moves (1) or (2).
   The permuter reached 86.8% from the 85.4% file only by moving `v.y = 0` after the normalize call, which
   changes behaviour; rejected. */
typedef struct { float x, y, z; } RwV3d;
typedef struct { RwV3d right; unsigned int flags; RwV3d up; unsigned int pad1; RwV3d at; unsigned int pad2; RwV3d pos; unsigned int pad3; } RwMatrix;
typedef struct { void *a; void *b; void *c; void *d; RwMatrix modelling; } RwFrame;
extern int DAT_00639d78;
int printf(const char *fmt, ...);
void FUN_00475770(float *p0, RwMatrix *p1);
double FUN_004a3384(double x); /* CRT acos, stack-argument entry */
float FUN_004c39b0(RwV3d *out, RwV3d *in);
#define DOT(a,b) ((a)->x * (b)->x + (a)->y * (b)->y + (a)->z * (b)->z)
// MATCH: 0x00405540 FUN_00405540
int FUN_00405540(float *out)
{
  RwMatrix *m;
  RwV3d v;
  RwV3d zaxis;
  float a[3];
  double d;

  if (DAT_00639d78 == 0) {
    return 0;
  }
  m = &((RwFrame *)*(int *)(DAT_00639d78 + 4))->modelling;
  *++out = m->pos.x;
  *++out = m->pos.y;
  *++out = m->pos.z;
  out++;
  v.y = m->up.y;
  v.z = m->up.z;
  v.x = 0.0f;
  FUN_004c39b0(&v, &v);
  d = FUN_004a3384((0, m->up.x) * v.x + v.y * m->up.y + v.z * m->up.z);
  d = d * 57.29578f;
  if (!(v.y < 0.0f)) d = -d;
  v.x = m->at.x;
  v.z = m->at.z;
  a[0] = (float)d;
  v.y = 0.0f;
  zaxis.x = 0.0f; zaxis.y = 0.0f; zaxis.z = 1.0f;
  FUN_004c39b0(&v, &v);
  d = FUN_004a3384(v.x * zaxis.x + (0, v.y * zaxis.y) + v.z * zaxis.z);
  d = d * 57.29578f;
  if (!(v.z > 0.0f)) d = -d;
  v.x = m->right.x;
  v.y = m->right.y;
  a[1] = (float)d;
  v.z = 0.0f;
  FUN_004c39b0(&v, &v);
  d = FUN_004a3384((0, v.x * m->right.x + v.y * m->right.y) + (0, m->right.z) * v.z);
  d = d * 57.29578f;
  if (!(v.x > 0.0f)) d = -d;
  a[2] = (float)d;
  printf("roll %f, pitch %f, yaw %f\n", a[2], a[0], a[1]);
  *out++ = a[0];
  *out++ = a[1];
  *out = a[2];
  FUN_00475770(a, m);
  printf("roll %f, pitch %f, yaw %f\n", a[0], a[1], a[2]);
  return 1;
}
