/* DDKOBJ-SPIKE CRT math probe (L-120 input). Built twice: XP DDK cl 13.00.9176 + its link/libc, and VS2022 + UCRT.
   Writes the float-rounded and the raw double results of sin/cos/atan2/sqrt (intrinsic form, the /O2 __CIxxx path)
   and sin/cos (function form, #pragma function) for N deterministic inputs, at x87 PC_24 and PC_53, to argv[1]. */
#include <stdio.h>
#include <math.h>
#include <float.h>

double f_sin(double);   /* fn.c: #pragma function(sin, cos) */
double f_cos(double);

static unsigned int s = 0x2468ace1u;
static unsigned int rnd(void) { s ^= s << 13; s ^= s >> 17; s ^= s << 5; return s; }

int main(int argc, char **argv)
{
    FILE *f = fopen(argv[1], "wb");
    int pcs[2], k, i, n = 4096;
    pcs[0] = _PC_24; pcs[1] = _PC_53;
    for (k = 0; k < 2; k++) {
        s = 0x2468ace1u;
        for (i = 0; i < n; i++) {
            float x, y;
            double r[6];
            unsigned int u = rnd();
            /* inputs: angles in [-8pi, 8pi] (float), plus pairs for atan2 and positives for sqrt */
            x = (float)(((double)(u & 0xffffff) / 16777216.0 - 0.5) * 50.2654824574);
            y = (float)(((double)(rnd() & 0xffffff) / 16777216.0 - 0.5) * 2000.0);
            _controlfp(pcs[k], _MCW_PC);
            r[0] = sin(x);
            r[1] = cos(x);
            r[2] = atan2(y, x);
            r[3] = sqrt(fabs(y));
            r[4] = f_sin(x);
            r[5] = f_cos(x);
            _controlfp(_PC_53, _MCW_PC);
            fwrite(&x, 4, 1, f);
            fwrite(r, 8, 6, f);
        }
    }
    fclose(f);
    return 0;
}
