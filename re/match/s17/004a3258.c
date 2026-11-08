// FLAGS: /O1
/* MATCH-S17 0x004a3258: CRT crt0dat.c doexit(code, quick, retcaller) (VS.NET 2002 form, no __try):
   _lockexit = _lock(8) FUN_004a787f, _C_Exit_Done DAT_007739dc, _C_Termination_Done DAT_007739d8, _exitflag
   DAT_007739d4, __onexitbegin/__onexitend DAT_008ab6d0/DAT_008ab6cc, __xp_a/__xp_z 0x005ea05c/0x005ea064,
   __xt_a/__xt_z 0x005ea068/0x005ea070, _unlockexit = _unlock(8) FUN_004a77eb, __crtExitProcess FUN_004a31b1.
   Section tables are arrays of unknown bound (MGLOBAL over a typedef T[]), see 0x004a31f3. */
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
typedef void (__cdecl *_PVFV)(void);
typedef _PVFV _PVFV_TAB[];
MGLOBAL(int, DAT_007739dc, 0x007739dc);
MGLOBAL(int, DAT_007739d8, 0x007739d8);
MGLOBAL(char, DAT_007739d4, 0x007739d4);
MGLOBAL(_PVFV *, DAT_008ab6d0, 0x008ab6d0);
MGLOBAL(_PVFV *, DAT_008ab6cc, 0x008ab6cc);
MGLOBAL(_PVFV_TAB, DAT_005ea05c, 0x005ea05c);
MGLOBAL(_PVFV_TAB, DAT_005ea064, 0x005ea064);
MGLOBAL(_PVFV_TAB, DAT_005ea068, 0x005ea068);
MGLOBAL(_PVFV_TAB, DAT_005ea070, 0x005ea070);
__declspec(dllimport) void *__stdcall GetCurrentProcess(void);
__declspec(dllimport) int __stdcall TerminateProcess(void *hProcess, unsigned int uExitCode);
void FUN_004a787f(int locknum);
void FUN_004a77eb(int locknum);
void FUN_004a31b1(int status);

static void _initterm(_PVFV *pfbegin, _PVFV *pfend)
{
    while (pfbegin < pfend) {
        if (*pfbegin != 0)
            (**pfbegin)();
        ++pfbegin;
    }
}

// MATCH: 0x004a3258 FUN_004a3258
void FUN_004a3258(int code, int quick, int retcaller)
{
    FUN_004a787f(8);
    if (DAT_007739dc == 1)
        TerminateProcess(GetCurrentProcess(), code);
    DAT_007739d8 = 1;
    DAT_007739d4 = (char)retcaller;
    if (!quick) {
        if (DAT_008ab6d0) {
            while (--DAT_008ab6cc >= DAT_008ab6d0)
                if (*DAT_008ab6cc != 0)
                    (**DAT_008ab6cc)();
        }
        _initterm(DAT_005ea05c, DAT_005ea064);
    }
    _initterm(DAT_005ea068, DAT_005ea070);
    if (retcaller) {
        FUN_004a77eb(8);
        return;
    }
    DAT_007739dc = 1;
    FUN_004a31b1(code);
}
