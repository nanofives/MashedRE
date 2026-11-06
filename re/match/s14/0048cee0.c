// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 0x0048cee0 (h7): 100% (140 B whole symbol byte-identical incl. jump and index tables). Raw 69.4%. The index arrives in EDI (Ghidra unaff_EDI): FUN_0048cee0 is a static callee taking int i (pilot finding 1), and this file holds a small stand-in caller Standin_0048e820 shaped like the real caller 0x0048e820 (index loop 0x11 down to 0, kept in EDI) so cl gives the callee its EDI convention; the stand-in is not the real 0x0048e820 (599 B, outside this slice). Case order in the switch: the 0xd/0xe/0x10 block before case 1 (sets the jump-table order). Residual: none. */
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
typedef struct Rec00722470 { int d[0x10]; } Rec00722470;
MGLOBALA(Rec00722470, DAT_00722470, 0x11, 0x00722470);
MGLOBALA(int, DAT_007668b0, 0x11, 0x007668b0);
MGLOBALA(char, DAT_00722250, 0x220, 0x00722250);
int FUN_0040bb30(char *p0);
void FUN_00476cb0(Rec00722470 *p0, int p1, int p2);
void FUN_004770c0(Rec00722470 *p0, int p1, int p2, int p3);
void FUN_004b6480(void *p0, int p1);

// MATCH: 0x0048cee0 _FUN_0048cee0
static void FUN_0048cee0(int i)
{
    int tex;

    tex = FUN_0040bb30("smoke");
    switch (i) {
    case 0xd:
    case 0xe:
    case 0x10:
        tex = FUN_0040bb30("wfall");
        break;
    case 1:
        tex = FUN_0040bb30("flatshad");
        break;
    }
    FUN_004770c0(&DAT_00722470[i], 0x817, 0x100, tex);
    FUN_00476cb0(&DAT_00722470[i], 5, 6);
    DAT_007668b0[i] = 0;
}

/* stand-in caller (the real caller 0x0048e820 keeps the index in EDI) */
void Standin_0048e820(void)
{
    int i;

    i = 0x11;
    do {
        i--;
        FUN_004b6480(&DAT_00722250[i * 0x20], 0x100);
        FUN_0048cee0(i);
    } while (i != 0);
}
