// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 (h1) 0x0045c550: 37.3% (regs 74.7%, ops 93.3%), 73/75 ins. Raw converter 33.3%.
   Residual (b): in the chain loop the original reloads table[id].next at the loop top and tests a second,
   separate load (into edi) at the bottom; ours carries the bottom load into the next iteration, one load
   fewer, so every register after it shifts (n in esi instead of edx). Tried do-while, while, for, an
   explicit break, an __inline accessor, a char * cast access, the value stored through ids[n]; permuter 269
   variants (its 53.3% best stores ids[n] before the update, which changes behaviour, rejected). */
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
typedef struct Obj0045c550 {
    int pad00;
    int f04;
    int pad08;
    int state;
    int pad10;
    int f14;
    int pad18[4];
    int f28;
} Obj0045c550;
typedef struct Arg0045c550 {
    int pad00;
    int f04;
    int f08;
} Arg0045c550;
typedef struct Node0045c550 {
    int next;
    int pad04[9];
} Node0045c550;
MGLOBALA(Node0045c550, DAT_005fcb74, 1, 0x005fcb74);
MGLOBALA(Obj0045c550 *, DAT_0088f680, 4, 0x0088f680);

// MATCH: 0x0045c550 FUN_0045c550
int FUN_0045c550(int id, Arg0045c550 *arg)
{
    int ids[20];
    int b[20];
    int c[20];
    int n;
    int i;
    Obj0045c550 **pp;
    Obj0045c550 *o;

    if (DAT_005fcb74[id].next == 0)
        return 0;
    n = 0;
    while (DAT_005fcb74[id].next != 0) {
        id = DAT_005fcb74[id].next;
        ids[n] = id;
        if (id == 0x25) {
            b[n] = b[n - 1];
            c[n] = c[n - 1];
        } else if (n > 0) {
            b[n] = c[n - 1];
            c[n] = b[n - 1];
        } else {
            b[0] = arg->f08;
            c[0] = arg->f04;
        }
        n++;
    }
    for (pp = DAT_0088f680; (int)pp < (int)(DAT_0088f680 + 4); pp++) {
        o = *pp;
        if (o != 0 && o->state == 1) {
            for (i = 0; i < n; i++) {
                if (o->f28 == ids[i] && o->f04 == b[i] && o->f14 == c[i])
                    return 1;
            }
        }
    }
    return 0;
}
