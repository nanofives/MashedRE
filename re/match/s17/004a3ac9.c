// FLAGS: /O1 /GS
/* MATCH-S17 0x004a3ac9: CRT strtok.c strtok (VS.NET 2002 libcmt, multithread: saved pointer in
   _getptd()->_token at +0x18). /O1 /GS (cookie 0x00616038 xor return address, check 0x004a2be9).
   100% first try from the CRT source. */
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
typedef struct { char pad[0x18]; unsigned char *_token; } tiddata_t;
tiddata_t * __cdecl FUN_004a8828(void);
// MATCH: 0x004a3ac9 FUN_004a3ac9
char * __cdecl FUN_004a3ac9(char *string, const char *control)
{
    unsigned char *str;
    const unsigned char *ctrl = (const unsigned char *)control;
    unsigned char map[32];
    int count;
    tiddata_t *ptd = FUN_004a8828();

    for (count = 0; count < 32; count++)
        map[count] = 0;

    do {
        map[*ctrl >> 3] |= (1 << (*ctrl & 7));
    } while (*ctrl++);

    if (string)
        str = (unsigned char *)string;
    else
        str = ptd->_token;

    while ((map[*str >> 3] & (1 << (*str & 7))) && *str)
        str++;

    string = (char *)str;

    for ( ; *str; str++)
        if (map[*str >> 3] & (1 << (*str & 7))) {
            *str++ = '\0';
            break;
        }

    ptd->_token = str;

    if ((unsigned char *)string == str)
        return 0;
    else
        return string;
}
