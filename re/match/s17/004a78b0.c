// FLAGS: /O1
/* MATCH-S17 0x004a78b0: CRT pre-init loop (libmatch: no library pair; hooks.csv CrtPreInitLoop). Calls every
   non-NULL PVFV in [0x005e7b84, 0x005e7b84) under __try / __except(EXCEPTION_EXECUTE_HANDLER); the table begin
   and end are the same address in this image, so the loop runs zero times. /O1.
   100%: whole obj symbol 68 B BYTE-IDENTICAL (Ghidra's 61 B body stops before the loop back-jump and the
   __SEH_epilog tail). relocheck BAD only on the scope-table const (its filter/handler entries are relocations into
   this function, compared unmasked): tool limit. */
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
typedef void (__cdecl *PVFV)(void);
MGLOBALA(PVFV, DAT_005e7b84, 1, 0x005e7b84);
// MATCH: 0x004a78b0 FUN_004a78b0
void __cdecl FUN_004a78b0(void)
{
    PVFV *pf;

    for (pf = DAT_005e7b84; pf < DAT_005e7b84; pf++) {
        __try {
            if (*pf != 0)
                (**pf)();
        }
        __except (1) {
        }
    }
}
