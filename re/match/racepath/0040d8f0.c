// FLAGS: /O2 /GS /Ob1
/* MATCH-RACEPATH-FRESH 0x0040d8f0 (first hand source; raw g2s 40.2%): 337 B whole symbol BYTE-IDENTICAL
   (Ghidra body 336 B stops before the ret). Closed by: a float/V3 matrix type in place of a char buffer
   (no /GS cookie), the slot table as a 2-D global int[4][4] indexed [car][i] (gives the cmp [esi],0
   reload and EBP for i), the create step as `if ((slot = FUN_00477b60()) != 0)`, the zero path as a
   4-step loop (xor ecx,ecx + 4 register stores), and the literals 450.0f (0x005ccd70) / 0.05f (0x005cc9a0).
   Residual: none. */
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
typedef struct { float x, y, z; } RwV3d;
typedef struct { RwV3d right; int flags; RwV3d up; int pad1; RwV3d at; int pad2; RwV3d pos; int pad3; } RwMatrix;
MGLOBAL(int, DAT_00636ae0, 0x00636ae0);
MGLOBAL(unsigned char, DAT_007f101c, 0x007f101c);
typedef int Slots4[4];
MGLOBALA(Slots4, DAT_0063b968, 4, 0x0063b968);
int  FUN_0046d6a0(float *out, int car);
int  FUN_0046cc10(int *out, int car);
int  FUN_0046d2e0(int car, int a, int *out);
int  FUN_00426c00(void);
int  FUN_00477b60(void);
void FUN_00477e00(int fx, int on);
void FUN_0041f260(int car, RwMatrix *out);
int  FUN_0041f290(int car, int i);
int  FUN_00477be0(int fx, RwV3d *pos, RwMatrix *m, float a, float b);

// MATCH: 0x0040d8f0 _FUN_0040d8f0
void FUN_0040d8f0(int car)
{
    int col;
    int state;
    float speed;
    RwV3d pos;
    RwMatrix mat;
    int on;
    int i;

    on = 0;
    if (DAT_00636ae0 != 0)
        return;
    FUN_0046d6a0(&speed, car);
    FUN_0046cc10(&state, car);
    if (speed > 450.0f && state == 4)
        on = 1;
    FUN_0046d2e0(car, 0, &col);
    if (col == -0x7fbfc0)
        on = 0;
    if ((FUN_00426c00() == 0x24 && col == -0x237f80 && state == 4) || on) {
        for (i = 0; i < 4; i++) {
            if (DAT_0063b968[car][i] == 0) {
                if ((DAT_0063b968[car][i] = FUN_00477b60()) != 0)
                    FUN_00477e00(DAT_0063b968[car][i], 1);
            }
            if (DAT_0063b968[car][i] != 0 && (DAT_007f101c & 2)) {
                FUN_0041f260(car, &mat);
                pos = *(RwV3d *)(FUN_0041f290(car, i) + 0x30);
                pos.y = pos.y - 0.05f;
                DAT_0063b968[car][i] = FUN_00477be0(DAT_0063b968[car][i], &pos, &mat, 0.08f, 2.0f);
            }
        }
    } else {
        for (i = 0; i < 4; i++)
            DAT_0063b968[car][i] = 0;
    }
}
