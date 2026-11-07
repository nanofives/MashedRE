// FLAGS: /O1
/* MATCH-S17 0x004ae635: CRT setlocal.c _setlocale_get_all (VS.NET 2002 libcmt). /O1.
   __lc_category[6] = { char *catname; char *locale; int (*init)(void); } at 0x00616d20.
   Best 82.8% (regs 85.9%, ops 93.8%). The original keeps two pointer IVs (prev in ebx, cur in esi, signed jl
   bound 0x616d60). The CRT index loop (i=2..LC_MAX, cat[i-1]/cat[i]) gives one IV: 68.8-71.9%. This file's
   prev/cur struct-pointer pair is the closest found; `if (!same) return` puts the free path out of line as in the
   original. Permuter 95 compiles no gain. Residual class (b). */
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
typedef struct { char *catname; char *locale; int (__cdecl *init)(void); } lccat_t;
MGLOBALA(lccat_t, DAT_00616d20, 6, 0x00616d20);
void * __cdecl FUN_004a45fb(unsigned int size);
void __cdecl FUN_004a460d(void *p);
void __cdecl FUN_004ae4b6(char *dst, int n, ...);
char * __cdecl FUN_004ac570(char *dst, const char *src);
int __cdecl FUN_004af340(const char *a, const char *b);
// MATCH: 0x004ae635 FUN_004ae635
char * __cdecl FUN_004ae635(void)
{
    int same = 1;

    if ((DAT_00616d20[0].locale == 0) &&
        ((DAT_00616d20[0].locale = (char *)FUN_004a45fb(0x351)) == 0))
        return 0;

    DAT_00616d20[0].locale[0] = '\0';
    FUN_004ae4b6(DAT_00616d20[0].locale, 3, DAT_00616d20[1].catname, "=", DAT_00616d20[1].locale);
    {
        lccat_t *p, *q;
        for (p = &DAT_00616d20[1]; p < &DAT_00616d20[5]; p = q) {
            q = p + 1;
            FUN_004ac570(DAT_00616d20[0].locale, ";");
            if (FUN_004af340(p->locale, q->locale))
                same = 0;
            FUN_004ae4b6(DAT_00616d20[0].locale, 3, q->catname, "=", q->locale);
        }
    }
    if (!same)
        return DAT_00616d20[0].locale;
    FUN_004a460d(DAT_00616d20[0].locale);
    DAT_00616d20[0].locale = 0;
    return DAT_00616d20[2].locale;
}
