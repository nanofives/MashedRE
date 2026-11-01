// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00423dd0 (h7): 100% exact. Same shape as pilot re/match/pilot/f_00423e60.c `_v_loop2`
   (team slot loop over 4 slots, stride 0x10 at 0x007f1a18; record field at 0x00899a48, stride 0x138).
   Raw converter (unrolled loop) was 56.1%; the rolled for-loop with the team read hoisted into a local
   gives the compiler's own unroll. Plus the leading `FUN_0042f6a0() == 4` -> return 0 guard. Residual: none. */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
typedef struct { int team; int pad[3]; } Slot;      /* stride 0x10 at 0x007f1a18 */
typedef struct { int value; int pad[0x4d]; } Rec;   /* stride 0x138 at 0x00899a48 */
MGLOBALA(Slot, DAT_007f1a18, 4, 0x007f1a18);
MGLOBALA(Rec, DAT_00899a48, 4, 0x00899a48);
int FUN_0042f500(void);
int FUN_0042f6a0(void);

// MATCH: 0x00423dd0 FUN_00423dd0
int FUN_00423dd0(int param_1)
{
    int j, r;
    if (FUN_0042f6a0() == 4)
        return 0;
    if (FUN_0042f500()) {
        int t = DAT_007f1a18[param_1].team;
        r = DAT_00899a48[param_1].value;
        for (j = 0; j < 4; j++)
            if (DAT_007f1a18[j].team == t && j != param_1)
                r += DAT_00899a48[j].value;
    } else
        r = DAT_00899a48[param_1].value;
    return r;
}
