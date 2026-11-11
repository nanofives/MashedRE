/* L-120 (b) feasibility: a VS2022 exe that keeps its own CRT and also links the XP DDK libc sin/cos objects renamed
   by ddkcrt_rename.py (prefix _ddkcrt). Writes the same record layout as probe.c, with the sin/cos slots taken from the
   renamed DDK bodies: intrinsic entry (__CIsin / __CIcos: argument and result in ST0) and function entry (_sin/_cos).
   atan2 and sqrt stay VS2022's (control columns). Compare with the pure-DDK probe output. */
#include <stdio.h>
#include <math.h>
#include <float.h>

double ddkcrt_sin(double);
double ddkcrt_cos(double);
void ddkcrt__CIsin(void);
void ddkcrt__CIcos(void);

static double ci_sin(double x) { double r; __asm { fld x } __asm { call ddkcrt__CIsin } __asm { fstp r } return r; }
static double ci_cos(double x) { double r; __asm { fld x } __asm { call ddkcrt__CIcos } __asm { fstp r } return r; }

static unsigned int s = 0x2468ace1u;
static unsigned int rnd(void) { s ^= s << 13; s ^= s >> 17; s ^= s << 5; return s; }

int main(int argc, char **argv)
{
    FILE *f = fopen(argv[1], "wb");
    int pcs[2], k, i, n = 4096;
    unsigned int old;
    pcs[0] = _PC_24; pcs[1] = _PC_53;
    for (k = 0; k < 2; k++) {
        s = 0x2468ace1u;
        for (i = 0; i < n; i++) {
            float x, y;
            double r[6];
            unsigned int u = rnd();
            x = (float)(((double)(u & 0xffffff) / 16777216.0 - 0.5) * 50.2654824574);
            y = (float)(((double)(rnd() & 0xffffff) / 16777216.0 - 0.5) * 2000.0);
            _controlfp_s(&old, pcs[k], _MCW_PC);
            r[0] = ci_sin(x);
            r[1] = ci_cos(x);
            r[2] = atan2(y, x);
            r[3] = sqrt(fabs(y));
            r[4] = ddkcrt_sin(x);
            r[5] = ddkcrt_cos(x);
            _controlfp_s(&old, _PC_53, _MCW_PC);
            fwrite(&x, 4, 1, f);
            fwrite(r, 8, 6, f);
        }
    }
    fclose(f);
    return 0;
}
