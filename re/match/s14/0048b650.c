// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 0x0048b650 (h7): 100%. Raw 25.2%. What moved it: 0x40-entry pool of 0x18-byte records at 0x0071fa40 with use flags at 0x007200d0; the free-slot loop written in the function with break (an inline helper made a different p register); the fallback scans records 0..7 for the largest +0x10 (compiler unrolls it); 0x005d757c (0.0f) and 0x005cc56c (0.1f) are compiler float literals, not globals (the literal made the first compare a memory fcomp and the fld/fadd order); position copied as a 12-byte struct (gives the mov eax,ecx copy pointer and the reload); i = 0 before idx = 0 (permuter). Residual: none. */
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
typedef struct Pool0071fa40 {
    float pos[3];   /* +0x00 */
    int f0c;        /* +0x0c */
    float age;      /* +0x10 */
    int data;       /* +0x14 */
} Pool0071fa40;
MGLOBALA(Pool0071fa40, DAT_0071fa40, 0x40, 0x0071fa40);
MGLOBALA(int, DAT_007200d0, 0x40, 0x007200d0);

typedef struct Vec3_0048b650 { float x, y, z; } Vec3_0048b650;

// MATCH: 0x0048b650 _FUN_0048b650
void FUN_0048b650(float *pos, int data)
{
    Pool0071fa40 *p;
    float best;
    int i;
    int idx;

    best = 0.0f;
    i = 0;
    p = 0;
    idx = 0;
    while (i < 0x40) {
        if (DAT_007200d0[i] == 0) {
            p = &DAT_0071fa40[i];
            DAT_007200d0[i] = 1;
            break;
        }
        i++;
    }
    if (p == 0) {
        for (i = 0; i < 8; i++) {
            if (DAT_0071fa40[i].age > best) {
                best = DAT_0071fa40[i].age;
                idx = i;
            }
        }
        p = &DAT_0071fa40[idx];
        DAT_007200d0[idx] = 1;
    }
    p->data = data;
    p->age = 0;
    p->f0c = 0;
    *(Vec3_0048b650 *)p->pos = *(Vec3_0048b650 *)pos;
    p->pos[1] = p->pos[1] + 0.1f;
}
