// FLAGS: /O1
/* MATCH-S17 0x004a57e4: CRT seccinit.c __security_init_cookie: __security_cookie DAT_00616038 = FILETIME halves ^
   pid ^ tid ^ tick ^ perf-counter halves, 0 -> DEFAULT_SECURITY_COOKIE 0xBB40E64E.
   Best 92.6% (regs 92.6%), /O1 (/O1 /Oy-, /O1 /GS, /Ogsy tie, /O2 /Ox 52%). Residual class (b): only the epilogue
   pop esi sits before the jne in ours and after the conditional store in the original. Operand order of both xor
   pairs is set by writing Low ^ High. Tried 12 rewrites of the tail (test cookie or the global, early return,
   assignment in the condition, split xors) and match_permute (127 compiles, no gain). */
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
typedef struct { unsigned long dwLowDateTime; unsigned long dwHighDateTime; } FT;
typedef struct { unsigned long LowPart; long HighPart; } LI;
MGLOBAL(unsigned int, DAT_00616038, 0x00616038);
__declspec(dllimport) void __stdcall GetSystemTimeAsFileTime(FT *p);
__declspec(dllimport) unsigned long __stdcall GetCurrentProcessId(void);
__declspec(dllimport) unsigned long __stdcall GetCurrentThreadId(void);
__declspec(dllimport) unsigned long __stdcall GetTickCount(void);
__declspec(dllimport) int __stdcall QueryPerformanceCounter(LI *p);
// MATCH: 0x004a57e4 FUN_004a57e4
void FUN_004a57e4(void)
{
    unsigned int cookie;
    FT systime;
    LI perfctr;

    GetSystemTimeAsFileTime(&systime);
    cookie = systime.dwLowDateTime ^ systime.dwHighDateTime;
    cookie ^= GetCurrentProcessId();
    cookie ^= GetCurrentThreadId();
    cookie ^= GetTickCount();
    QueryPerformanceCounter(&perfctr);
    cookie ^= perfctr.LowPart ^ perfctr.HighPart;
    DAT_00616038 = cookie;
    if (cookie == 0)
        DAT_00616038 = 0xBB40E64E;
}
