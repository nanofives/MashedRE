// FLAGS: /O2 /GS /Ob1
/* MATCH-RACEPATH-FRESH 0x00411ae0 Ghost::PlaybackTick (first hand source; raw g2s 43.9%): NOT CLOSED,
   97.2% insn, 27 B differ (497 B whole symbol). Two identity matrices through the RW
   rwMatrixSetIdentity macro shape (chained stores right to left, flags |= 0x20003 read uninitialised,
   as the binary does at 0x00411b74). The time select needs a local and the `== 0` branch first
   (0x00411aee jne). Residual: the final sum is reassociated: ours (bb1c + d) + b7a, the original
   (b7a + bb1c) + d at 0x00411b43; and the &best store is scheduled before the 0x0063bb0c test. Tried 19
   term orders, locals for the sum and the difference, int/uint widths, ternary forms. */
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
typedef struct { V3 right; uint flags; V3 up; uint pad1; V3 at; uint pad2; V3 pos; uint pad3; } RwMatrix;
typedef struct { int a; int b; RwMatrix m; } GhostXform;
typedef struct { RwMatrix *best; RwMatrix *ghost; } GhostPair;
#define MatSetIdentity(m) \
    { (m)->right.x = (m)->up.y = (m)->at.z = 1.0f; \
      (m)->right.y = (m)->right.z = (m)->up.x = 0.0f; \
      (m)->up.z = (m)->at.x = (m)->at.y = 0.0f; \
      (m)->pos.x = (m)->pos.y = (m)->pos.z = 0.0f; \
      (m)->flags = (m)->flags | 0x20003; }
MGLOBAL(int, DAT_0063bb24, 0x0063bb24);
MGLOBAL(int, DAT_0063bb0c, 0x0063bb0c);
MGLOBAL(int, DAT_0063bb10, 0x0063bb10);
MGLOBAL(uint, DAT_0063bb1c, 0x0063bb1c);
MGLOBAL(uchar, DAT_007f1038, 0x007f1038);
MGLOBAL(uchar, DAT_007f1039, 0x007f1039);
MGLOBAL(uchar, DAT_007f1079, 0x007f1079);
MGLOBAL(uchar, DAT_007f107a, 0x007f107a);
int  FUN_0042f6a0(void);
void FUN_00482c10(int replay, RwMatrix *out, uint t);
void FUN_0041a9b0(GhostXform *x);
void FUN_0041ad00(GhostPair *p);

// MATCH: 0x00411ae0 _FUN_00411ae0
void FUN_00411ae0(int unused, uint t)
{
    GhostPair pair;
    GhostXform gx;
    RwMatrix best;
    uint v;

    if (DAT_0063bb24 == 0)
        v = t;
    else
        v = DAT_007f107a + DAT_0063bb1c + (-(DAT_007f1038 / 10) + DAT_007f1039 / 10 - DAT_007f1079);
    DAT_0063bb1c = v;
    if (DAT_0063bb10 != 0 && DAT_0063bb1c > *(uint *)(DAT_0063bb10 + 0x174))
        DAT_0063bb1c = *(uint *)(DAT_0063bb10 + 0x174);
    if (FUN_0042f6a0() - 2 == 0) {
        pair.ghost = &gx.m;
        pair.best = &best;
        gx.a = 1;
        gx.b = 0;
        MatSetIdentity(&best);
        MatSetIdentity(&gx.m);
        if (DAT_0063bb0c != 0)
            FUN_00482c10(DAT_0063bb0c, &gx.m, DAT_0063bb1c);
        if (DAT_0063bb10 != 0)
            FUN_00482c10(DAT_0063bb10, &best, DAT_0063bb1c);
        FUN_0041a9b0(&gx);
        FUN_0041ad00(&pair);
    }
}
