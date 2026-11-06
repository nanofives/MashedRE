// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00458630 (h6): 100%, whole obj symbol byte-identical (360 B incl. the 21-entry jump table at
   0x00458744 that Ghidra's 274 B size leaves out). Raw converter 83.5%. What moved it: (1) case bodies in the
   original's layout order 9, 6/12/13, 7, 10, 11, 2, 17, 16, 18, 19, 21, 22 (read from the jump table);
   (2) the seven unused values 3, 4, 5, 8, 14, 15, 20 written as explicit `case N: return 0;` labels: without them
   (or with them grouped on `default`) the compiler emits a byte index table + 13-entry table instead of the
   original's flat 21-entry table. Strings are the original's pooled .rdata strings (0x005ce568 "flamethrower" is
   used by cases 2 and 16). Residual: none. */
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
MGLOBAL(void *, DAT_0068b9ac, 0x0068b9ac);
MGLOBALA(char, s_machinegun_005ce59c, 11, 0x005ce59c);
MGLOBALA(char, s_mine_005ce594, 5, 0x005ce594);
MGLOBALA(char, s_mortar_005ce58c, 7, 0x005ce58c);
MGLOBALA(char, s_depthcharge_005ce580, 12, 0x005ce580);
MGLOBALA(char, s_missile_005ce578, 8, 0x005ce578);
MGLOBALA(char, s_flamethrower_005ce568, 13, 0x005ce568);
MGLOBALA(char, s_shotgun_005ce560, 8, 0x005ce560);
MGLOBALA(char, s_flare_005ce558, 6, 0x005ce558);
MGLOBALA(char, s_oil_005ce4fc, 4, 0x005ce4fc);
MGLOBALA(char, s_chaos_005ce550, 6, 0x005ce550);
MGLOBALA(char, s_airstrike_005cd130, 10, 0x005cd130);
void *FUN_004c5c00(void *dict, char *name);
// MATCH: 0x00458630 FUN_00458630
void *FUN_00458630(int type)
{
    switch (type) {
    case 9:
        return FUN_004c5c00(DAT_0068b9ac, s_machinegun_005ce59c);
    case 6:
    case 12:
    case 13:
        return FUN_004c5c00(DAT_0068b9ac, s_mine_005ce594);
    case 7:
        return FUN_004c5c00(DAT_0068b9ac, s_mortar_005ce58c);
    case 10:
        return FUN_004c5c00(DAT_0068b9ac, s_depthcharge_005ce580);
    case 11:
        return FUN_004c5c00(DAT_0068b9ac, s_missile_005ce578);
    case 2:
        return FUN_004c5c00(DAT_0068b9ac, s_flamethrower_005ce568);
    case 17:
        return FUN_004c5c00(DAT_0068b9ac, s_shotgun_005ce560);
    case 16:
        return FUN_004c5c00(DAT_0068b9ac, s_flamethrower_005ce568);
    case 18:
        return FUN_004c5c00(DAT_0068b9ac, s_flare_005ce558);
    case 19:
        return FUN_004c5c00(DAT_0068b9ac, s_oil_005ce4fc);
    case 21:
        return FUN_004c5c00(DAT_0068b9ac, s_chaos_005ce550);
    case 22:
        return FUN_004c5c00(DAT_0068b9ac, s_airstrike_005cd130);
    case 3:
        return 0;
    case 4:
        return 0;
    case 5:
        return 0;
    case 8:
        return 0;
    case 14:
        return 0;
    case 15:
        return 0;
    case 20:
        return 0;
    }
    return 0;
}
