// FLAGS: /O1
/* MATCH-S17 0x004a467e: CRT calloc.c calloc (VS.NET 2002 libcmt, V6 small-block heap path with
   __try/__finally around __sbh_alloc_block). /O1. The __finally funclet is 0x004a4728 (own file).
   100%: whole obj symbol 187 B BYTE-IDENTICAL (relocations masked), incl. the funclet and the return tail after
   it; Ghidra's 170 B body stops at the funclet. */
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
MGLOBAL(int, DAT_008aa6a0, 0x008aa6a0);
MGLOBAL(unsigned int, DAT_008aa68c, 0x008aa68c);
MGLOBAL(void *, DAT_008aa69c, 0x008aa69c);
MGLOBAL(int, DAT_00773c34, 0x00773c34);
__declspec(dllimport) void * __stdcall HeapAlloc(void *hHeap, unsigned long dwFlags, unsigned long dwBytes);
void __cdecl FUN_004a787f(int locknum);
void __cdecl FUN_004a77eb(int locknum);
void * __cdecl FUN_004aac76(unsigned int size);
void * __cdecl FUN_004aaf90(void *dst, int val, unsigned int count);
int __cdecl FUN_004aaf72(unsigned int size);
// MATCH: 0x004a467e FUN_004a467e
void * __cdecl FUN_004a467e(unsigned int num, unsigned int size)
{
    unsigned int size_orig;
    void *pvReturn;

    size_orig = size = size * num;

    if (size == 0)
        size = 1;

    for (;;) {
        pvReturn = 0;

        if (size <= 0xffffffe0) {
            if (DAT_008aa6a0 == 3) {
                if (size <= 0xffffffe0)
                    size = (size + 0xf) & ~0xf;

                if (size_orig <= DAT_008aa68c) {
                    FUN_004a787f(4);
                    __try {
                        pvReturn = FUN_004aac76(size_orig);
                    }
                    __finally {
                        FUN_004a77eb(4);
                    }
                    if (pvReturn != 0)
                        FUN_004aaf90(pvReturn, 0, size_orig);
                }
            }

            if (pvReturn == 0)
                pvReturn = HeapAlloc(DAT_008aa69c, 8, size);
        }

        if (pvReturn || DAT_00773c34 == 0)
            return pvReturn;

        if (!FUN_004aaf72(size))
            return 0;
    }
}
