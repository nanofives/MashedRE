// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 0x00487020 (h4): best 96.3% (regs 96.3%). Moves: FUN_0045bfe0 takes no args (returns a global), so the call is FUN_004b4b60(FUN_0045bfe0(), &sphere, &hit); the 40-byte block of the hit record typed float[10] (a char[40] put a /GS cookie in, 80.5%); sphere radius stored before the centre copy; .rdata 0x005cc9bc (0.8f) and 0x005ce18c (0.02f) as literals; height test as !(h.n.y < 0.8f). Residual: the x87 stack keeps -h.d alive for r.z (fld n.z; fmul st(2); fstp st(2)) where the original multiplies it in place (fxch; fmul [n.z]; fxch). Tried t*n / n*t per lane, no temp (-h.d per lane), negating h.d in place, copy-then-scale, inline VScale/VAdd helpers, 189 permuter variants. Class (b). */
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
typedef struct V3 { float x, y, z; } V3;
typedef struct Sph { V3 c; float r; } Sph;
typedef struct Hit { V3 n; float f0c; float blk[10]; float d; float f3c; } Hit;
typedef struct Mat { float m[16]; } Mat;
int FUN_0045bfe0(void);
int FUN_004b4b60(int w, Sph *s, Hit *h);
int FUN_0045c350(Hit *h, V3 *p);
void FUN_004b5080(Mat *m, void *a, V3 *p);
void FUN_00486610(Mat *m, int p);

// MATCH: 0x00487020 FUN_00487020
void FUN_00487020(V3 *param_1, int param_2)
{
    V3 r;
    Sph s;
    Hit h;
    Mat m;
    float t;
    s.r = 0.5f;
    s.c = *param_1;
    if (FUN_004b4b60(FUN_0045bfe0(), &s, &h) != 0 && !(h.n.y < 0.8f)) {
        if (FUN_0045c350(&h, param_1) == 0) {
            t = -h.d;
            r.x = h.n.x * t;
            r.y = h.n.y * t;
            r.z = h.n.z * t;
            r.x = s.c.x + r.x;
            r.y = s.c.y + r.y;
            r.z = s.c.z + r.z;
            r.x = r.x + h.n.x * 0.02f;
            r.y = r.y + h.n.y * 0.02f;
            r.z = r.z + h.n.z * 0.02f;
            FUN_004b5080(&m, h.blk, &r);
            FUN_00486610(&m, param_2);
        }
    }
}
