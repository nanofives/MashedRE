// FLAGS: /O2 /GS /Ob1
/* MATCH-S19 0x004c4680, rewritten by RACE-LOAD-004E99B0 (run 57, 2026-11-27) from the disassembly of 0x004c4680..0x004c4a47
   (Ghidra pool slot Mashed_pool0, read-only). The earlier raw g2s.py body (34.5%) used separate float locals as 3-vectors
   through pointers (pfVar[1], pfVar[2]) and converted the +0x0c flags word float -> uint -> float; under VS2022 that wrote
   over its own return slot (object-build arm RO_ROLOADRWC-rl5: ret to 0 after the 0x004cf2b0 -> 0x004c4670 call).
   Frame (S = esp after `sub esp,0x3c`): rows A = src[0..2] at S+0x00, B = src[4..6] at S+0x0c, C = src[8..10] at S+0x18,
   scale kA at S+0x24 and kB at S+0x28 (later reused for |B.C| and |C.A|), src[13] / src[14] at S+0x34 / S+0x38, src[12] in EBP.
   Args: dst, src. 0x004c3b90(len2) gives each row's scale (0x004c4702 / 0x004c474a / 0x004c4792); DAT_005d757c threshold
   (0x004c47bc). Pair choice (u, v, w) = (A, B, C) by default, (B, C, A) / (C, A, B) by the tests at 0x004c47c9..0x004c48b2;
   w = v x u, normalised, then v = w x u... as the stores at 0x004c48fb..0x004c49c9 do; dst rows from A / B / C, dst +0x30..+0x38
   from src[12..14], dst +0x0c = (dst +0x0c & 0xfffdffff) | 3 (0x004c4a35). */
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
MGLOBAL(float, DAT_005d757c, 0x005d757c);
float FUN_004c3b90(float len2);

// MATCH: 0x004c4680 FUN_004c4680
void FUN_004c4680(float *dst, float *src)
{
    float m[3][3];
    float kA, kB, kC, dBC, dCA, dAB, px, py, pz, k;
    float *u, *v, *w;

    m[0][0] = src[0]; m[0][1] = src[1]; m[0][2] = src[2];
    m[1][0] = src[4]; m[1][1] = src[5]; m[1][2] = src[6];
    m[2][0] = src[8]; m[2][1] = src[9]; m[2][2] = src[10];
    px = src[12]; py = src[13]; pz = src[14];
    kA = FUN_004c3b90(m[0][1] * m[0][1] + m[0][0] * m[0][0] + m[0][2] * m[0][2]);
    m[0][0] = kA * m[0][0]; m[0][1] = kA * m[0][1]; m[0][2] = kA * m[0][2];
    kB = FUN_004c3b90(m[1][1] * m[1][1] + m[1][0] * m[1][0] + m[1][2] * m[1][2]);
    m[1][0] = kB * m[1][0]; m[1][1] = kB * m[1][1]; m[1][2] = kB * m[1][2];
    kC = FUN_004c3b90(m[2][1] * m[2][1] + m[2][0] * m[2][0] + m[2][2] * m[2][2]);
    m[2][0] = m[2][0] * kC; m[2][1] = m[2][1] * kC; m[2][2] = m[2][2] * kC;
    u = m[0]; v = m[1]; w = m[2];
    if (kA <= DAT_005d757c) {
        u = m[1]; v = m[2]; w = m[0];
    } else if (kB <= DAT_005d757c) {
        u = m[2]; v = m[0]; w = m[1];
    } else if (kC > DAT_005d757c) {
        dBC = m[2][1] * m[1][1] + m[2][0] * m[1][0] + m[2][2] * m[1][2];
        if (dBC < DAT_005d757c) dBC = -dBC;
        dCA = m[2][1] * m[0][1] + m[2][0] * m[0][0] + m[2][2] * m[0][2];
        if (dCA < DAT_005d757c) dCA = -dCA;
        dAB = m[1][1] * m[0][1] + m[1][0] * m[0][0] + m[1][2] * m[0][2];
        if (dAB < DAT_005d757c) dAB = -dAB;
        if (dBC >= dCA) {
            if (dCA < dAB) { u = m[2]; v = m[0]; w = m[1]; }
        } else if (dBC < dAB) {
            u = m[1]; v = m[2]; w = m[0];
        }
    }
    w[0] = v[2] * u[1] - v[1] * u[2];
    w[1] = u[2] * v[0] - v[2] * u[0];
    w[2] = v[1] * u[0] - u[1] * v[0];
    k = FUN_004c3b90(w[0] * w[0] + w[1] * w[1] + w[2] * w[2]);
    w[0] = k * w[0]; w[1] = w[1] * k; w[2] = w[2] * k;
    v[0] = u[2] * w[1] - u[1] * w[2];
    v[1] = w[2] * u[0] - u[2] * w[0];
    v[2] = u[1] * w[0] - w[1] * u[0];
    k = FUN_004c3b90(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    v[0] = k * v[0]; v[1] = v[1] * k; v[2] = v[2] * k;
    dst[0] = m[0][0]; dst[1] = m[0][1]; dst[2] = m[0][2];
    dst[4] = m[1][0]; dst[5] = m[1][1]; dst[6] = m[1][2];
    dst[8] = m[2][0]; dst[9] = m[2][1]; dst[10] = m[2][2];
    dst[12] = px; dst[13] = py; dst[14] = pz;
    *(unsigned int *)(dst + 3) = (*(unsigned int *)(dst + 3) & 0xfffdffff) | 3;
}
