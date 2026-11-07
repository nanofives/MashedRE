// FLAGS: /O1 /Oy-
/* MATCH-S17 0x004a787f: CRT mlock.c _lock (VS.NET 2002 libcmt). /O1 /Oy- (original keeps the ebp frame).
   _mtinitlocknum = 0x004a7800, _amsg_exit = 0x004a4b6e (0x11 = _RT_LOCK). 100%. */
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
__declspec(dllimport) void __stdcall EnterCriticalSection(void *lpCriticalSection);
int __cdecl FUN_004a7800(int locknum);
void __cdecl FUN_004a4b6e(int rterrnum);
// MATCH: 0x004a787f FUN_004a787f
void __cdecl FUN_004a787f(int locknum)
{
    if (DAT_00616408[locknum].lock == 0) {
        if (!FUN_004a7800(locknum))
            FUN_004a4b6e(0x11);
    }
    EnterCriticalSection(DAT_00616408[locknum].lock);
}
