// FLAGS: /O1 /GS
/* MATCH-S17 0x004ab8d6: CRT crt0msg.c _NMSG_WRITE (VS.NET 2002 libcmt). /O1 /GS (cookie 0x00616038 xor return
   address). rterrs[18] = { int rterrno; char *rterrtxt; } at 0x00616890; __error_mode 0x007739f0, __app_type
   0x006160c8; _alloca -> __alloca_probe 0x004a3440, __crtMessageBoxA 0x004ac45c.
   100%: the table loop bound must be unsigned (sizeof(rterrs)/sizeof(rterrs[0]), jb); a plain 18 gave jl (99.1%). */
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
typedef struct { int rterrno; char *rterrtxt; } rterr_t;
MGLOBALA(rterr_t, DAT_00616890, 18, 0x00616890);
MGLOBAL(int, DAT_007739f0, 0x007739f0);
MGLOBAL(int, DAT_006160c8, 0x006160c8);
__declspec(dllimport) void * __stdcall GetStdHandle(unsigned long nStdHandle);
__declspec(dllimport) int __stdcall WriteFile(void *hFile, const void *lpBuffer, unsigned long n,
    unsigned long *lpWritten, void *lpOverlapped);
__declspec(dllimport) unsigned long __stdcall GetModuleFileNameA(void *hModule, char *lpFilename, unsigned long nSize);
void * __cdecl _alloca(unsigned int size);
unsigned int __cdecl FUN_004a9410(const char *s);
char * __cdecl FUN_004ac560(char *dst, const char *src);
char * __cdecl FUN_004ac570(char *dst, const char *src);
char * __cdecl FUN_004a34b0(char *dst, const char *src, unsigned int n);
int __cdecl FUN_004ac45c(const char *text, const char *caption, unsigned int type);
// MATCH: 0x004ab8d6 FUN_004ab8d6
void __cdecl FUN_004ab8d6(int rterrnum)
{
    int tblindx;
    unsigned long bytes_written;

    for (tblindx = 0; tblindx < sizeof(DAT_00616890) / sizeof(DAT_00616890[0]); tblindx++)
        if (rterrnum == DAT_00616890[tblindx].rterrno)
            break;

    if (rterrnum == DAT_00616890[tblindx].rterrno) {
        if ((DAT_007739f0 == 1) ||
            ((DAT_007739f0 == 0) && (DAT_006160c8 == 1))) {
            WriteFile(GetStdHandle((unsigned long)-12),
                      DAT_00616890[tblindx].rterrtxt,
                      (unsigned long)FUN_004a9410(DAT_00616890[tblindx].rterrtxt),
                      &bytes_written,
                      0);
        }
        else if (rterrnum != 0xfc) {
            char progname[261];
            char *pch;
            char *outmsg;

            progname[260] = '\0';
            if (!GetModuleFileNameA(0, progname, 260))
                FUN_004ac560(progname, "<program name unknown>");

            pch = progname;

            if (FUN_004a9410(pch) + 1 > 60) {
                pch += FUN_004a9410(progname) + 1 - 60;
                FUN_004a34b0(pch, "...", 3);
            }

            outmsg = (char *)_alloca(FUN_004a9410(pch) + FUN_004a9410(DAT_00616890[tblindx].rterrtxt) + 26 + 2);
            FUN_004ac560(outmsg, "Runtime Error!\n\nProgram: ");
            FUN_004ac570(outmsg, pch);
            FUN_004ac570(outmsg, "\n\n");
            FUN_004ac570(outmsg, DAT_00616890[tblindx].rterrtxt);
            FUN_004ac45c(outmsg, "Microsoft Visual C++ Runtime Library", 0x12010);
        }
    }
}
