// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 (h1) 0x0045bfa0: 100% (regs 100%), 11/11 ins, bytes identical. The file also holds 0x0045baa0
   (100%, 13/13 ins) as a static callee: it takes the type id in ESI (pilot finding 1). 0x0045baa0 needed the
   guarded do-while pointer walk (`i = 0; if (0 < n) { t = table; do ... }`); the for form put the pointer
   load before the guard (92.3%), the index form scaled with shl (37.5%). Residual: none. */
/* ---- MATCH-S10 preamble (same text in every re/match/s10 source; not part of any function body) ----
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
struct Slot0045bfa0;
typedef struct PwType0045bfa0 {
    int type;
    int (*activate)(struct Slot0045bfa0 *s);
    void *f08[14];
} PwType0045bfa0;
typedef struct Slot0045bfa0 {
    int pad00[0x2a];
    PwType0045bfa0 *type;
    int state;
    int index;
} Slot0045bfa0;
MGLOBAL(int, DAT_005f9bd8, 0x005f9bd8);
MGLOBALA(PwType0045bfa0, DAT_005f9998, 8, 0x005f9998);

// MATCH: 0x0045baa0 FUN_0045baa0
static PwType0045bfa0 *FUN_0045baa0(int type)
{
    int i;
    PwType0045bfa0 *t;
    i = 0;
    if (0 < DAT_005f9bd8) {
        t = DAT_005f9998;
        do {
            if (type == t->type)
                return t;
            i++;
            t++;
        } while (i < DAT_005f9bd8);
    }
    return 0;
}

// MATCH: 0x0045bfa0 FUN_0045bfa0
void FUN_0045bfa0(Slot0045bfa0 *s, int type)
{
    s->type = FUN_0045baa0(type);
    s->state = s->type->activate(s);
}
