/* DDKOBJ-CRTMATH: __ftol value probe. Built as DDK cl 13.00.9176 (calls libc __ftol), VS2022 /arch:IA32 (calls
   _ftol2_sse or inline fistp) and VS2022 default SSE2 (cvttsd2si). Writes (int)x for N deterministic doubles and
   floats plus the edge values (+-0.5, +-1, 2^31 +- epsilon, +-2^31, NaN, +-Inf, subnormal) at x87 PC_24 and PC_53 to argv[1].
   Output record: 4 bytes input float bits | 8 bytes input double | int r_double | int r_float | int r_scaled. */
#include <stdio.h>
#include <string.h>
#include <float.h>

static unsigned int s = 0x13579bdfu;
static unsigned int rnd(void) { s ^= s << 13; s ^= s >> 17; s ^= s << 5; return s; }

static volatile double vd;
static volatile float vf;

int main(int argc, char **argv)
{
    FILE *f = fopen(argv[1], "wb");
    int pcs[2], k, i, n = 4096, total;
    static const double edge[] = {0.0, -0.0, 0.5, -0.5, 0.9999999, -0.9999999, 1.0, -1.0, 1.5, -1.5, 2.5, -2.5,
        2147483647.0, 2147483647.5, 2147483648.0, -2147483648.0, -2147483648.5, -2147483649.0, 4294967296.0,
        1e30, -1e30, 1e-300, 4.9e-324, 16777216.0, 16777217.0, 16777215.5};
    unsigned int zero = 0;
    double nan_d, inf_d;
    unsigned int nb[2] = {0, 0x7ff80000u};
    unsigned int ib[2] = {0, 0x7ff00000u};
    memcpy(&nan_d, nb, 8);
    memcpy(&inf_d, ib, 8);
    (void)zero;
    pcs[0] = _PC_24; pcs[1] = _PC_53;
    total = n + (int)(sizeof(edge) / sizeof(edge[0])) + 4;
    for (k = 0; k < 2; k++) {
        s = 0x13579bdfu;
        for (i = 0; i < total; i++) {
            double d;
            float fl;
            int r0, r1, r2;
            unsigned int fb;
            if (i < n) {
                unsigned int u = rnd();
                int sh = (int)(rnd() % 40u);        /* magnitude 2^-8 .. 2^31 */
                d = ((double)(u & 0xffffff) / 16777216.0 - 0.5) * 2.0 * (double)(1u << (sh < 31 ? sh : 31)) / 256.0;
            } else if (i - n < (int)(sizeof(edge) / sizeof(edge[0]))) {
                d = edge[i - n];
            } else {
                d = (i - n) % 4 == 0 ? nan_d : (i - n) % 4 == 1 ? inf_d : (i - n) % 4 == 2 ? -inf_d : -nan_d;
            }
            fl = (float)d;
            _controlfp(pcs[k], _MCW_PC);
            vd = d;
            vf = fl;
            r0 = (int)vd;
            r1 = (int)vf;
            r2 = (int)(vd * 3.0 + 0.25);
            _controlfp(_PC_53, _MCW_PC);
            memcpy(&fb, &fl, 4);
            fwrite(&fb, 4, 1, f);
            fwrite(&d, 8, 1, f);
            fwrite(&r0, 4, 1, f);
            fwrite(&r1, 4, 1, f);
            fwrite(&r2, 4, 1, f);
        }
    }
    fclose(f);
    return 0;
}
