// FLAGS: /O2 /GS /Ob1
/* a6 0x004093b0 (496 B): 100% BYTES-OK. Steps from 73%: (1) DAT_005d757c is the compiler's 0.0f literal
   constant: writing `0.0f` instead of the extern made the v.x/v.y compares stay on the FPU (+19%);
   (2) `a = -(acos(d)*57.2958f - 90.0); a += 90.0f;` as two statements (one-liner folds to fsubr);
   (3) the dot product as `d = y*y; d += x*x; d += z*z;` (one expression reorders the terms);
   (4) a copy `cur = h` of the parameter for the loop handle (swaps ebx/edi to the original).
   FUN_004a3384 = CRT acos stack entry, declared as a plain function (pilot finding 5). Calls static callee
   0x00409330 as a plain cdecl extern. */
typedef struct { float x, y, z; } V3;
extern V3 DAT_006146f0;
int FUN_00409330(int v, int n);
float *FUN_00426cc0(int h);
double FUN_004a3384(double x);  /* acos, CRT stack entry */
float FUN_004c39b0(V3 *out, V3 *in);
// MATCH: 0x004093b0 FUN_004093b0
int FUN_004093b0(int h, float *out_avg, float *out_var)
{
    float acc, sum, prev, d, a, t;
    float *p;
    V3 v;
    int i, cur;
    cur = h;
    acc = 0.0f;
    sum = 0.0f;
    prev = 0.0f;
    for (i = 0; i < 3; i++) {
        p = FUN_00426cc0(cur);
        v.x = p[0];
        v.y = 0.0f;
        v.z = p[2];
        if (v.x == 0.0f && v.z == 0.0f)
            return 1;
        FUN_004c39b0(&v, &v);
        d = DAT_006146f0.y * v.y;
        d += DAT_006146f0.x * v.x;
        d += DAT_006146f0.z * v.z;
        if (d < -1.0f)
            d = -1.0f;
        else if (d > 1.0f)
            d = 1.0f;
        a = -(FUN_004a3384(d) * 57.2958f - 90.0);
        a += 90.0f;
        v.x = DAT_006146f0.z * p[1] - DAT_006146f0.y * p[2];
        v.y = DAT_006146f0.x * p[2] - DAT_006146f0.z * p[0];
        v.z = DAT_006146f0.y * p[0] - DAT_006146f0.x * p[1];
        if (v.y < 0.0f)
            a = 360.0f - a;
        while (a > 90.0f)
            a -= 90.0f;
        if (a > 45.0f)
            a -= 90.0f;
        sum += a;
        if (i != 0) {
            t = prev - a;
            if (t < 0.0f)
                t = -t;
            acc += t;
        }
        prev = a;
        cur = FUN_00409330(cur, 0);
    }
    *out_avg = sum * 0.33333334f;
    *out_var = acc;
    return 1;
}
