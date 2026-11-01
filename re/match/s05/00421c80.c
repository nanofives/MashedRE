// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00421c80: by hand; 0x00421c80 shows 94.5% only because Ghidra's size stops before its epilogue: match.py --bytes reports the whole obj symbol BYTE-IDENTICAL. 0x00421980 100%.
   This file holds a copy of the static callee 0x00421980 (atomic in EAX; also held
   by 004219f0.c), defined before its caller. */
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
typedef struct Ent00421c80 { int a0, a4, a8; int clump; int obj; int vis; } Ent00421c80;
typedef struct Grp00421c80 { Ent00421c80 e[10]; unsigned char bits[4]; int count; int set; int rec; } Grp00421c80;
typedef struct Rec00421c80 { Grp00421c80 g[2]; int model; int idx; } Rec00421c80;
MGLOBAL(int, DAT_006ce274, 0x006ce274);
int FUN_0055dec0(int p0);
void FUN_00559ee0(int p0, int p1);
void FUN_0057c370(int p0);
void FUN_004e7e30(int p0, int p1);
void FUN_004c0c20(int p0);
void FUN_004e6e00(int p0);
void FUN_004b6520(void *p0, int p1);

// MATCH: 0x00421980 _FUN_00421980
static void FUN_00421980(int atomic)
{
    FUN_00559ee0(FUN_0055dec0(DAT_006ce274), atomic);
}

// MATCH: 0x00421c80 _FUN_00421c80
void FUN_00421c80(Rec00421c80 *rec)
{
    int j, i;
    for (j = 0; j < 2; j++) {
        for (i = 0; i < 10; i++) {
            Ent00421c80 *e = &rec->g[j].e[i];
            if (e->obj != 0) {
                if (*(int *)(e->obj + 0x24) != 0)
                    FUN_00421980(e->obj);
                FUN_0057c370(e->obj);
            }
            if (e->clump != 0) {
                int fr = *(int *)(e->clump + 4);
                FUN_004e7e30(e->clump, 0);
                FUN_004c0c20(fr);
            }
        }
    }
    FUN_004e6e00(rec->model);
    FUN_004b6520(rec, 0x208);
}
