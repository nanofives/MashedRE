// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x004260e0 (525 B): 100.0% exact (regs 100, ops 100), relocheck OK.
   Source = the MATCH-PILOT a7 hand match (re/match/pilot/a7_004260e0.c), moved to the S06 form.
   What moved the score there: strcpy/strcat intrinsics; the strcpy source is the const array "c:" @0x005cd4ac
   (a literal would give a fixed word+byte move, the original runs a byte loop); `sep = dos ? "\\" : "/";` then
   `if (dos) { strcpy; strcat(buf, sep); }`: the ternary's "/" arm is laid after the ret, and constant propagation
   turns strcat(buf, sep) inside if(dos) into the fixed 2-byte move of "\\" @0x005cc4dc. strcat(buf, "") is the
   byte store from 0x005cc4de. Strings: "ToastArt" @0x005cd4a0, "tracks" @0x005cd498, "/" @0x005cc4d8.
   Buffer @0x008991e0 (declared length 256: the length is not visible in this function [UNCERTAIN]).
   Residual: none. */
/* ---- MATCH-S06 preamble (same text in every re/match/s06 source; not part of any function body) ----
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
MGLOBALA(char, DAT_008991e0, 256, 0x008991e0);
MGLOBALA(const char, DAT_005cd4ac, 4, 0x005cd4ac);
char *strcpy(char *, const char *);
char *strcat(char *, const char *);
#pragma intrinsic(strcpy, strcat)
// MATCH: 0x004260e0 FUN_004260e0
char *FUN_004260e0(int dos, char *a, char *b, char *c)
{
    const char *sep;
    DAT_008991e0[0] = 0;
    sep = dos ? "\\" : "/";
    if (dos) {
        strcpy(DAT_008991e0, DAT_005cd4ac);
        strcat(DAT_008991e0, sep);
    }
    strcat(DAT_008991e0, "ToastArt");
    strcat(DAT_008991e0, sep);
    strcat(DAT_008991e0, "");
    strcat(DAT_008991e0, sep);
    strcat(DAT_008991e0, "tracks");
    strcat(DAT_008991e0, sep);
    strcat(DAT_008991e0, a);
    if (*b)
        strcat(DAT_008991e0, sep);
    strcat(DAT_008991e0, b);
    if (*c)
        strcat(DAT_008991e0, c);
    return DAT_008991e0;
}
