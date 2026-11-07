// FLAGS: /O1 /GS
/* MATCH-S17 0x004a583a: CRT secfail.c __security_error_handler(code, data): user_handler DAT_007739f4 under
   __try/__except(1), else builds "<type>\n\nProgram: <name>\n\n<msg>" in an _alloca buffer and shows it with
   __crtMessageBoxA FUN_004ac45c (MB_OK|MB_ICONHAND|MB_SETFOREGROUND|MB_TASKMODAL 0x12010), then _exit(3)
   FUN_004a333c. strcpy FUN_004ac560, strcat FUN_004ac570, strlen FUN_004a9410, strncpy FUN_004a34b0 (MASM).
   Matched: whole obj symbol BYTE-IDENTICAL over 329 B (relocations masked), /O1 /GS (the cookie is the VC7 form,
   __security_cookie ^ return address at [ebp-0x1c]; no check call since _exit does not return). match.py prints
   96.1% only because the Ghidra size (320 B) stops before add esp,0x3c / push 3 / call _exit, so the jmp to that
   tail is outside its range. relocheck OK (___security_cookie, __alloca_probe, __SEH_prolog unchecked: compiler
   generated). The _alloca size is written as subtextlen + strlen(pch) + 12. */
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
typedef void (__cdecl *_secerr_handler_func)(int, void *);
MGLOBAL(_secerr_handler_func, DAT_007739f4, 0x007739f4);
__declspec(dllimport) unsigned long __stdcall GetModuleFileNameA(void *hModule, char *lpFilename, unsigned long nSize);
char *FUN_004ac560(char *dst, const char *src);
char *FUN_004ac570(char *dst, const char *src);
unsigned int FUN_004a9410(const char *s);
char *FUN_004a34b0(char *dst, const char *src, unsigned int n);
int FUN_004ac45c(const char *text, const char *caption, unsigned int type);
__declspec(noreturn) void FUN_004a333c(int status);
void *_alloca(unsigned int size);
#pragma intrinsic(_alloca)

#define BOVERRUN_TYPE "Buffer overrun detected!"
#define BOVERRUN_MSG "A buffer overrun has been detected which has corrupted the program's\ninternal state.  The program cannot safely continue execution and must\nnow be terminated.\n"
#define UNKNOWN_TYPE "Unknown security failure detected!"
#define UNKNOWN_MSG "A security error of unknown cause has been detected which has\ncorrupted the program's internal state.  The program cannot safely\ncontinue execution and must now be terminated.\n"
#define PROGINTRO "Program: "
#define DOTDOTDOT "..."
#define MAXLINELEN 60

// MATCH: 0x004a583a FUN_004a583a
void FUN_004a583a(int code, void *data)
{
    char progname[260 + 1];
    char *pch;
    char *outmsg;
    const char *type;
    const char *msg;
    unsigned int subtextlen;

    if (DAT_007739f4 != 0) {
        __try {
            DAT_007739f4(code, data);
        }
        __except (1) {
        }
    }
    else {
        switch (code) {
        default:
            type = UNKNOWN_TYPE;
            msg = UNKNOWN_MSG;
            subtextlen = sizeof(UNKNOWN_TYPE) + sizeof(UNKNOWN_MSG);
            break;
        case 1:
            type = BOVERRUN_TYPE;
            msg = BOVERRUN_MSG;
            subtextlen = sizeof(BOVERRUN_TYPE) + sizeof(BOVERRUN_MSG);
            break;
        }

        progname[260] = '\0';
        if (!GetModuleFileNameA(0, progname, 260))
            FUN_004ac560(progname, "<program name unknown>");

        pch = progname;

        if (sizeof(PROGINTRO) + FUN_004a9410(progname) + 1 > MAXLINELEN) {
            pch += (sizeof(PROGINTRO) + FUN_004a9410(progname) + 1) - MAXLINELEN;
            FUN_004a34b0(pch, DOTDOTDOT, sizeof(DOTDOTDOT) - 1);
        }

        outmsg = (char *)_alloca(subtextlen - 1 + 2 + sizeof(PROGINTRO) - 1 + FUN_004a9410(pch) + 2);

        FUN_004ac560(outmsg, type);
        FUN_004ac570(outmsg, "\n\n");
        FUN_004ac570(outmsg, PROGINTRO);
        FUN_004ac570(outmsg, pch);
        FUN_004ac570(outmsg, "\n\n");
        FUN_004ac570(outmsg, msg);

        FUN_004ac45c(outmsg, "Microsoft Visual C++ Runtime Library", 0x12010);
    }
    FUN_004a333c(3);
}
