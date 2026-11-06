// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x0046c5f0 (210 B): 100.0% (regs 100, ops 100), from re/match/pilot/a5_0046c5f0.c (h3).
   Register-passed args (pilot finding 1): the function is static, defined before its caller, /Ob1, so cl
   gives it a in EAX, c in ECX, b in EDX, out in ESI. Parameter order (a, c, b, out) picks those registers.
   The real caller 0x0046da80 is not in this file; a stand-in caller under #ifndef MATCH_PORT keeps the
   static alive and its convention (the bytes of 0x0046c5f0 are identical; the file is not the real TU). */
/* ---- MATCH-S11 preamble (same text in every re/match/s11 source; not part of any function body) ----
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
float FUN_004c3b30(float x);
// MATCH: 0x0046c5f0 FUN_0046c5f0
static void FUN_0046c5f0(float *a, float *c, float *b, float *out)
{
    float ax = a[0] - c[0], ay = a[1] - c[1], az = a[2] - c[2];
    float bx = a[0] - b[0], by = a[1] - b[1], bz = a[2] - b[2];
    float nx = bz * ay - by * az;
    float ny = bx * az - bz * ax;
    float nz = by * ax - bx * ay;
    float l = nx * nx + ny * ny + nz * nz;
    if (l >= 0.0f) {
        l = FUN_004c3b30(l);
        if (l != 0.0f) {
            l = 1.0f / l;
            out[0] = nx * l;
            out[1] = ny * l;
            out[2] = l * nz;
        }
    }
}

#ifndef MATCH_PORT
/* stand-in caller (not the real 0x0046da80): only makes cl give FUN_0046c5f0 its register convention */
MGLOBALA(float, DAT_0088e620, 12, 0x0088e620);
void h3_standin_0046c5f0(float *out)
{
    FUN_0046c5f0(&DAT_0088e620[0], &DAT_0088e620[6], &DAT_0088e620[9], out);
}
#endif
