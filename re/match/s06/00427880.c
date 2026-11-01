// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 h5 0x00427880 (67 B). Best 100.0% (regs 100.0%). Raw converter 86.7%.
   Static callee with the name in EAX (the real caller 0x00427d41 does `lea eax,[esp+0x28]; call`). Callees
   0x004cc230 (3 args), 0x004cc5e0 (4), 0x00554390 (1), 0x004cc160 (2), all cdecl. The real caller is not in
   this slice: FUN_00427880_caller below is a STAND-IN (not matched, not shipped). Residual: none. */
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
#ifdef __cplusplus
#define MATCH_STATIC
#else
#define MATCH_STATIC static
#endif
MGLOBAL(int, DAT_0067d838, 0x0067d838);

void *FUN_004cc230(int type, int mode, const char *name);
int FUN_004cc5e0(void *stream, int id, void *a, void *b);
int FUN_00554390(void *stream);
int FUN_004cc160(void *stream, void *a);

// MATCH: 0x00427880 FUN_00427880
MATCH_STATIC int FUN_00427880(const char *name)
{
    int result = 0;
    void *s = FUN_004cc230(2, 1, name);
    if (s) {
        if (FUN_004cc5e0(s, 0x199, 0, 0)) {
            result = FUN_00554390(s);
            FUN_004cc160(s, 0);
        }
    }
    return result;
}

#ifndef __cplusplus
/* stand-in caller (not matched, not shipped): passes name in EAX as 0x00427d41 does */
void FUN_00427880_caller(const char *src)
{
    char name[64];
    int i;
    for (i = 0; i < 63 && src[i]; i++) name[i] = src[i];
    name[i] = 0;
    DAT_0067d838 = FUN_00427880(name);
}
#endif
