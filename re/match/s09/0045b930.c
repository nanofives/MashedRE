// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x0045b930 (h10): 100% by bytes. Ghidra size 83 stops before the epilogue (add esp,4 / pop esi / ret
   at 0x0045b981..0x0045b985), so the instruction score reads 90.0% (27/30); match.py --bytes: 86 B whole symbol,
   BYTE-IDENTICAL. Table at 0x005f99b0 is 0x40-byte entries with a function pointer first (count DAT_005f9bd8,
   re-read every pass). Residual: none. */
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
typedef void (*Fn45b930)(void);
typedef struct Entry45b930 {
    Fn45b930 m_fn;
    int m_pad[15];
} Entry45b930;
MGLOBAL(int, DAT_005f9bd8, 0x005f9bd8);
MGLOBALA(Entry45b930, PTR_FUN_005f99b0, 8, 0x005f99b0);
MGLOBAL(int, DAT_0068d4e0, 0x0068d4e0);

void FUN_00412010(void);
void FUN_0045b350(void);
void FUN_00458880(void);
void FUN_004593b0(void);
void FUN_00404820(void);
int FUN_004c5930(int p0);

// MATCH: 0x0045b930 FUN_0045b930
void FUN_0045b930(void)
{
    int i;

    FUN_00412010();
    FUN_0045b350();
    FUN_00458880();
    FUN_004593b0();
    FUN_00404820();
    for (i = 0; i < DAT_005f9bd8; i++) {
        if (PTR_FUN_005f99b0[i].m_fn != 0) {
            PTR_FUN_005f99b0[i].m_fn();
        }
    }
    FUN_004c5930(DAT_0068d4e0);
}
