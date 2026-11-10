// FLAGS: /O2 /GS /Ob1
/* MATCH-RACEPATH-FRESH3 0x00414570 (first hand source; raw g2s 10.5%): 93.0%, 156 B differ, NOT CLOSED.
   Literals read from the anchored image (180/360/20/1000/2500/7/0.1/-1/1/57.2958/30). Moved it: acos as
   `q = (float)FUN_004a3384(q) * 57.2958f; if (q < 30.0f)` (fmul dword at 0x00414878; the cast inside the
   compare gives fld const; fmul st(1)); `k = skill * 0.1f` as one local (fld st(0); fmulp at 0x004147de);
   a float local for c->z in the dv statements (0x004146ed load order); the dot product split as
   `q = a + b; q += c` (cl reorders the single expression). Residual: `dist` gets bestd's frame slot
   (S+0x10 against the original's S+0x20, 4 operands) and out[1] is kept in ST0 (fst) where the original
   stores and reloads it (0x004147f1..0x004147fe), which shortens the symbol by 3 B and shifts every later
   jump. Tried: dist in block scope, bestd in block scope, d reused for dist, dv as float[3], out / tgt as
   V2 structs, dblgreedy (166 variants, no gain). */
/* ---- MATCH-S07 preamble (same text in every re/match/s07 source; not part of any function body) ----
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
typedef struct { float x, y, z; } V3;
typedef struct { char pad[0x30]; float x, y, z; } CarPos;
typedef struct { int skill; char pad[0x74 - 4]; } AiSkill74;   /* 0x74 B at 0x0089a4c0 */
MGLOBALA(AiSkill74, DAT_0089a4c0, 4, 0x0089a4c0);
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);
int   FUN_0046d4a0(CarPos **out, int car);
float FUN_00443440(int p1, float *pos, float r, float *ang, int z);
int   FUN_0046d6d0(float *p0, int p1);
int   FUN_0046d510(V3 *out, int car);
float FUN_004c39b0(V3 *out, V3 *in);
float FUN_004c3ac0(V3 *v);
int   FUN_00426bb0(void);
float FUN_00408a50(int i);
int   FUN_0046c7b0(int i);
int   FUN_0040e470(int i);
void  FUN_0046cbb0(int i, int *a, int *b);
double FUN_004a3384(double x);  /* acos, CRT stack entry */

// MATCH: 0x00414570 _FUN_00414570
int FUN_00414570(int p1, float *tgt, float *out, int car)
{
    float q;
    CarPos *c;
    float ang;
    float bestd;
    float spd;
    int st;
    float myprog;
    float dist;
    int tmp;
    float pos[2];
    V3 dv;
    V3 fwd;
    int lap;
    int best;
    int i;
    float d;
    float dx;
    float dz;
    float k;

    FUN_0046d4a0(&c, car);
    pos[0] = c->x;
    pos[1] = c->z;
    FUN_00443440(p1, pos, 7.5f, &ang, 0);
    if (ang > 180.0f)
        ang = 360.0f - ang;
    if (ang > 20.0f)
        return 0;
    FUN_0046d6d0(&spd, car);
    FUN_0046d510(&fwd, car);
    fwd.y = 0.0f;
    FUN_004c39b0(&fwd, &fwd);
    lap = FUN_00426bb0();
    myprog = FUN_00408a50(car);
    best = -1;
    bestd = 2.0f;
    for (i = 0; i < 4; i++) {
        if (i != car && FUN_0046c7b0(i) != 0 && FUN_0040e470(i) == 1) {
            FUN_0046cbb0(i, &st, &tmp);
            if (st == 0) {
                d = (q = FUN_00408a50(i)) - myprog;
                if ((d > 0.0f || d < 5 - lap) && d < bestd) {
                    bestd = d;
                    best = i;
                }
            }
        }
    }
    if (best != -1) {
        FUN_0046d4a0(&c, best);
        {float zz = c->z;
        dv.x = c->x - pos[0];
        dv.y = 0.0f;
        dv.z = zz - pos[1];}
        dist = FUN_004c3ac0(&dv);
        FUN_004c39b0(&dv, &dv);
        FUN_0046d6d0(&q, best);
        if (spd - q < 1000.0f && spd > 2500.0f && q > 2500.0f && dist < 7.0f) {
            FUN_0046d4a0(&c, best);
            out[0] = c->x;
            dist = c->z;
            out[1] = dist;
            dx = out[0] - tgt[0];
            dz = dist - tgt[1];
            if (DAT_007f0fd0 != 8) {
                k = DAT_0089a4c0[car].skill * 0.1f;
                dx = k * dx;
                dz = k * dz;
            }
            out[0] = dx + tgt[0];
            out[1] = dz + tgt[1];
            dv.x = out[0] - pos[0];
            dv.z = out[1] - pos[1];
            FUN_004c39b0(&dv, &dv);
            q = fwd.x * dv.x + dv.y * fwd.y;
            q += fwd.z * dv.z;
            if (q < -1.0f)
                q = -1.0f;
            else if (q > 1.0f)
                q = 1.0f;
            q = (float)FUN_004a3384(q) * 57.2958f;
            if (q < 30.0f)
                return 1;
        }
    }
    return 0;
}
