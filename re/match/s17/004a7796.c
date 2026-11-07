// FLAGS: /O1
/* MATCH-S17 0x004a7796: CRT mlock.c _mtdeletelocks (VS.NET 2002 libcmt). /O1. _free_crt = free 0x004a460d,
   kind 1 = lkPrealloc.
   Best 94.1% (regs 94.1%, ops 100%): the second loop's pointer IV is biased to &kind (mov eax,[esi-4] / cmp [esi],1)
   where the original addresses [esi] / [esi+4]. Tried: pcs vs direct reads in either loop (the direct form CSEs the
   loop bound into ebp, 70.3%), struct-pointer loop, nested ifs, continue, separate index, enum kind, file-static
   table, permuter 96 compiles. Residual class (b). relocheck BAD only from that bias (+4 displacement). */
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
typedef struct { void *lock; int kind; } locktab_t;
MGLOBALA(locktab_t, DAT_00616408, 36, 0x00616408);
__declspec(dllimport) void __stdcall DeleteCriticalSection(void *lpCriticalSection);
void __cdecl FUN_004a460d(void *p);
// MATCH: 0x004a7796 FUN_004a7796
void __cdecl FUN_004a7796(void)
{
    int locknum;
    void *pcs;

    for (locknum = 0; locknum < 36; locknum++) {
        if ((pcs = DAT_00616408[locknum].lock) != 0 && DAT_00616408[locknum].kind != 1) {
            DeleteCriticalSection(pcs);
            FUN_004a460d(pcs);
            DAT_00616408[locknum].lock = 0;
        }
    }
    for (locknum = 0; locknum < 36; locknum++) {
        if ((pcs = DAT_00616408[locknum].lock) != 0 && DAT_00616408[locknum].kind == 1)
            DeleteCriticalSection(pcs);
    }
}
