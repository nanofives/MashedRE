// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00456140 (h4): 100% (regs 100%), 35/35 ins on the first hand pass. Raw converter 34.3%.
   What moved it: the second parameter is a float scale copied into a float[2], the 4-byte 0xff colour is a
   local (the compiler packs it into the dead parameter's home slot, which Ghidra showed as param_2 reuse),
   0x00472650 returns float and its result goes straight into 0x00476a30(float). Residual: none. */
/* ---- MATCH-S09 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
typedef struct V3_00456140 { float x, y, z; } V3_00456140;
MGLOBALA(uchar, DAT_006887d0, 4, 0x006887d0);
void FUN_004769a0(V3_00456140 *p0);
void FUN_004769d0(float *p0);
void FUN_004769f0(uchar *p0);
void FUN_00474de0(V3_00456140 *p0, V3_00456140 *p1, float p2);
float FUN_00472650(int p0, float p1);
void FUN_00476a30(float p0);
void FUN_00476d00(void *p0);
// MATCH: 0x00456140 FUN_00456140
void FUN_00456140(V3_00456140 *p, float s)
{
    float sc[2];
    V3_00456140 out;
    uchar col[4];
    sc[0] = s;
    sc[1] = s;
    col[0] = 0xff;
    col[1] = 0xff;
    col[2] = 0xff;
    col[3] = 0xff;
    FUN_004769f0(col);
    FUN_004769d0(sc);
    FUN_00474de0(p, &out, 0.1f);
    FUN_004769a0(&out);
    FUN_00476a30(FUN_00472650(0, 360.0f));
    FUN_00476d00(DAT_006887d0);
}
