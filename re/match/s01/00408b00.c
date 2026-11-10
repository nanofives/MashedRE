// FLAGS: /O2 /GS /Ob1
/* MATCH-RACEPATH-SHIFT 2026-11-09: 90.2% / 214 bad B -> 97.0% / 93 bad B (first divergence +0x11f -> +0x1f5).
   (1) GY/GZ return the field as a comma rvalue `(0, p->y)`: the first value is spilled with fld/fstp to the dead
   out_ang slot as in the original. (2) `d.y = -GY(h, 3) + GY(h, 0)` (same value as GY(h,0) - GY(h,3), negation is
   exact): restores the original call order (n=0 first) and its fsubr. (3) s3/s4 back in x,y,z order (the old
   permuter order gave the same count before (1)/(2), 4 B worse after). Left: residual (2) below (the place==100
   compare and the edi/esi pops sit inside the s3.z/s4 multiplies in the original), one lea base/index swap
   (i = place - 4 + DAT_0063a5e4: 6 operand orders, no change) and the r.z fstp position in three tails.
   Permuter after these: 1532 compiles, no byte gain. The text below is the earlier state. */
/* a6 0x00408b00 (1883 B + 3 jump tables): best 90.2% (bad214/1932), g2s did not compile.
   Locals (esp0-rel): V2 a (-0x68), V2 up {0,1} (-0x60, its slot reused for r), V3 d (-0x54), base (-0x48),
   s1 (-0x3c), fwd (-0x30), s3 (-0x24), s2 (-0x18), s4 (-0xc). FUN_004c3c60 / FUN_004726b0 are 2D
   normalise / dot (V2*), FUN_004c39b0 the 3D normalise, FUN_004987b0 a printf-style logger.
   Steps: 55% plain per-component code -> 61.6% computing each case in place in r (r = base +- d, then
   r = r +- s1 ...; the in-place r is what produces the original fld/fadd x3, fstp r.z, fxch pattern) ->
   86.5% `*out = r; return;` inside every case (three distinct tails, as in the original) -> 88.8% with p3==4
   case 0 using `break` to the final `*out = r` (gives the original's tail placement) -> 89.8% with
   d.y / d.z through static __inline getters GY/GZ (forces the load before the second FUN_00426d00 call)
   -> 90.2% permuter (moved s4.z before s3.x; bytes identical count, scheduling only).
   Residual: (1) d.y/d.z: original spills the first value with fld/fstp to the out_ang arg slot, ours with
   mov/mov (inline getter) or keeps the pointer in edi (plain form). Tried *1.0f, +0.0f, double casts,
   temp float, `d.y -= ...`, pointer temps p0/p3, unary-minus forms: none gives the fld/fstp spill. (2) the
   `mov eax,place; cmp eax,0x64; pop edi; pop esi` sit 8 multiplies later in the original. (3) tail
   register rotation inside the three `*out = r` tails. Class (b) for (1), (a) candidate for (2)/(3)
   (scheduling and register rotation, 2058 permuter compiles without a change). */
typedef struct { float x, y; } V2;
typedef struct { float x, y, z; } V3;
extern int DAT_007f0fec;
extern int DAT_0063a5e0;
extern int DAT_0063a5e4;
extern int DAT_007f0fd0;
V3 *FUN_00426cb0(int h);
V3 *FUN_00426cc0(int h);
V3 *FUN_00426d00(int h, int n);
float FUN_004726b0(V2 *a, V2 *b);
void FUN_004987b0(const char *fmt, ...);
double FUN_004a3384(double x);  /* acos, CRT stack entry */
float FUN_004c39b0(V3 *out, V3 *in);
float FUN_004c3c60(V2 *out, V2 *in);
static __inline float GX(int h, int n) { return FUN_00426d00(h, n)->x; }
static __inline float GY(int h, int n) { return (0, FUN_00426d00(h, n)->y); }
static __inline float GZ(int h, int n) { return (0, FUN_00426d00(h, n)->z); }
// MATCH: 0x00408b00 FUN_00408b00
void FUN_00408b00(int h, int place, int ncars, V3 *out, float *out_ang)
{
    V2 a, up;
    V3 base, fwd, d, s1, s2, s3, s4, r;
    float ang;
    int i;

    up.x = 0.0f;
    up.y = 1.0f;
    if (DAT_007f0fec != DAT_0063a5e0) {
        if (++DAT_0063a5e4 >= ncars)
            DAT_0063a5e4 = 0;
    }
    DAT_0063a5e0 = DAT_007f0fec;
    if (DAT_007f0fd0 == 4 || DAT_007f0fd0 == 9 || DAT_007f0fd0 == 8)
        DAT_0063a5e4 = 3;
    base = *FUN_00426cb0(h);
    fwd = *FUN_00426cc0(h);
    a.x = fwd.x;
    a.y = fwd.z;
    FUN_004c3c60(&a, &a);
    ang = (float)FUN_004a3384(FUN_004726b0(&up, &a)) * 57.29578f;
    if (a.x < 0.0f)
        ang = 360.0f - ang;
    *out_ang = ang;
    d.x = FUN_00426d00(h, 0)->x - FUN_00426d00(h, 3)->x;
    d.y = -GY(h, 3) + GY(h, 0);
    d.z = -GZ(h, 3) + GZ(h, 0);
    FUN_004c39b0(&d, &d);
    s1.x = d.x * 0.4f;
    s1.y = d.y * 0.4f;
    s1.z = d.z * 0.4f;
    s2.x = fwd.x * 1.1f;
    s2.y = fwd.y * 1.1f;
    s2.z = fwd.z * 1.1f;
    s3.x = fwd.x * 2.2f;
    s3.y = fwd.y * 2.2f;
    s3.z = fwd.z * 2.2f;
    s4.x = fwd.x * 3.3f;
    s4.y = fwd.y * 3.3f;
    s4.z = fwd.z * 3.3f;
    if (place == 100) {
        r.x = base.x + s3.x;
        r.y = base.y + s3.y;
        r.z = base.z + s3.z;
        *out = r;
        return;
    }
    if (place < 4) {
        d.x *= 0.8f;
        d.y *= 0.8f;
        d.z *= 0.8f;
        s1.x *= 0.8f;
        s1.y *= 0.8f;
        s1.z *= 0.8f;
        switch (place) {
        case 0:
            r.x = base.x + d.x;
            r.y = base.y + d.y;
            r.z = base.z + d.z;
            r.x = r.x - s1.x;
            r.y = r.y - s1.y;
            r.z = r.z - s1.z;
            *out = r;
            return;
        case 1:
            r.x = base.x - d.x;
            r.y = base.y - d.y;
            r.z = base.z - d.z;
            r.x = r.x - s1.x;
            r.y = r.y - s1.y;
            r.z = r.z - s1.z;
            r.x = r.x - s2.x;
            r.y = r.y - s2.y;
            r.z = r.z - s2.z;
            *out = r;
            return;
        case 2:
            r.x = base.x + d.x;
            r.y = base.y + d.y;
            r.z = base.z + d.z;
            r.x = r.x + s1.x;
            r.y = r.y + s1.y;
            r.z = r.z + s1.z;
            r.x = r.x - s3.x;
            r.y = r.y - s3.y;
            r.z = r.z - s3.z;
            *out = r;
            return;
        case 3:
            r.x = base.x - d.x;
            r.y = base.y - d.y;
            r.z = base.z - d.z;
            r.x = r.x + s1.x;
            r.y = r.y + s1.y;
            r.z = r.z + s1.z;
            r.x = r.x - s4.x;
            r.y = r.y - s4.y;
            r.z = r.z - s4.z;
            *out = r;
            return;
        default:
            return;
        }
        *out = r;
        return;
    }
    i = place - 4 + DAT_0063a5e4;
    while (i >= ncars)
        i -= ncars;
    switch (ncars) {
    case 1:
        switch (i) {
        case 0:
            *out = base;
            return;
        default:
            FUN_004987b0("Invalid position (%d)\n", i);
            return;
        }
    case 2:
        d.x *= 0.6f;
        d.y *= 0.6f;
        d.z *= 0.6f;
        switch (i) {
        case 0:
            r.x = base.x + d.x;
            r.y = base.y + d.y;
            r.z = base.z + d.z;
            *out = r;
            return;
        case 1:
            r.x = base.x - d.x;
            r.y = base.y - d.y;
            r.z = base.z - d.z;
            *out = r;
            return;
        default:
            FUN_004987b0("Invalid position (%d)\n", i);
            return;
        }
        break;
    case 3:
        d.x *= 0.9f;
        d.y *= 0.9f;
        d.z *= 0.9f;
        switch (i) {
        case 0:
            r.x = base.x + d.x;
            r.y = base.y + d.y;
            r.z = base.z + d.z;
            *out = r;
            return;
        case 1:
            *out = base;
            return;
        case 2:
            r.x = base.x - d.x;
            r.y = base.y - d.y;
            r.z = base.z - d.z;
            *out = r;
            return;
        default:
            FUN_004987b0("Invalid position (%d)\n", i);
            return;
        }
        break;
    case 4:
        d.x *= 0.8f;
        d.y *= 0.8f;
        d.z *= 0.8f;
        s1.x *= 0.8f;
        s1.y *= 0.8f;
        s1.z *= 0.8f;
        switch (i) {
        case 0:
            r.x = base.x + d.x;
            r.y = base.y + d.y;
            r.z = base.z + d.z;
            r.x = r.x - s1.x;
            r.y = r.y - s1.y;
            r.z = r.z - s1.z;
            break;
        case 1:
            r.x = base.x - d.x;
            r.y = base.y - d.y;
            r.z = base.z - d.z;
            r.x = r.x - s1.x;
            r.y = r.y - s1.y;
            r.z = r.z - s1.z;
            r.x = r.x - s2.x;
            r.y = r.y - s2.y;
            r.z = r.z - s2.z;
            *out = r;
            return;
        case 2:
            r.x = base.x + d.x;
            r.y = base.y + d.y;
            r.z = base.z + d.z;
            r.x = r.x + s1.x;
            r.y = r.y + s1.y;
            r.z = r.z + s1.z;
            r.x = r.x - s3.x;
            r.y = r.y - s3.y;
            r.z = r.z - s3.z;
            *out = r;
            return;
        case 3:
            r.x = base.x - d.x;
            r.y = base.y - d.y;
            r.z = base.z - d.z;
            r.x = r.x + s1.x;
            r.y = r.y + s1.y;
            r.z = r.z + s1.z;
            r.x = r.x - s4.x;
            r.y = r.y - s4.y;
            r.z = r.z - s4.z;
            *out = r;
            return;
        default:
            FUN_004987b0("Invalid position (%d)\n", i);
            return;
        }
        break;
    default:
        FUN_004987b0("Invalid number of cars in race (%d)\n", ncars);
        return;
    }
    *out = r;
}
