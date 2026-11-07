// FLAGS: /O1
/* MATCH-S17 0x004abc53: CRT stdenvp.c _setenvp (VS.NET 2002 libcmt, _MBCS build: __initmbctable first). /O1. 100% first try from the CRT source. */
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
MGLOBAL(int, DAT_008ab6d4, 0x008ab6d4);
MGLOBAL(char *, DAT_007739e8, 0x007739e8);
MGLOBAL(char **, DAT_007739bc, 0x007739bc);
MGLOBAL(int, DAT_008ab6c8, 0x008ab6c8);
int __cdecl FUN_004af2b6(void);
unsigned int __cdecl FUN_004a9410(const char *s);
void * __cdecl FUN_004a45fb(unsigned int size);
void __cdecl FUN_004a460d(void *p);
char * __cdecl FUN_004ac560(char *dst, const char *src);
// MATCH: 0x004abc53 FUN_004abc53
int __cdecl FUN_004abc53(void)
{
    char *p;
    char **env;
    int numstrings;
    int cchars;

    if (!DAT_008ab6d4)
        FUN_004af2b6();

    numstrings = 0;
    p = DAT_007739e8;
    if (p == 0)
        return -1;

    while (*p != '\0') {
        if (*p != '=')
            ++numstrings;
        p += FUN_004a9410(p) + 1;
    }

    if ((DAT_007739bc = env = (char **)FUN_004a45fb((numstrings + 1) * sizeof(char *))) == 0)
        return -1;

    for (p = DAT_007739e8; *p != '\0'; p += cchars) {
        cchars = (int)FUN_004a9410(p) + 1;
        if (*p != '=') {
            if ((*env = (char *)FUN_004a45fb(cchars * sizeof(char))) == 0) {
                FUN_004a460d(DAT_007739bc);
                DAT_007739bc = 0;
                return -1;
            }
            FUN_004ac560(*env, p);
            env++;
        }
    }

    FUN_004a460d(DAT_007739e8);
    DAT_007739e8 = 0;
    *env = 0;
    DAT_008ab6c8 = 1;
    return 0;
}
