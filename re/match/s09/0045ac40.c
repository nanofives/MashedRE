// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x0045ac40 (h10): best 61.5% (regs 66.1%), 295 B. Raw g2s does not compile (__thiscall).
   Register args: FUN_0045ac40 is file-static (pilot finding 1), (a, b, p) gives a in EAX, b in ECX, p on the
   stack as at the real call site 0x0045af41 (inside 0x0045ae80); the file holds a stand-in caller
   (Caller_0045ac40, NOT matched). Table DAT_0068bd00 = Ent[4][5][5], 0x34-byte entries (+0x18 float, +0x30 int).
   Moved it: the inner j loop as an explicit goto loop (a for/while loop is unrolled x8 by 9176, 9%; goto 34%),
   ei as a pointer stepped with the outer counter i = 1..5 (48% -> 59.6%), compares as !(f >= c) and dj > d
   (test ah,1 / test ah,0x41 as in the original; 1.0f literal, pooled at 0x005cc320, scores the same as the
   global), permuter swap of the dy/dz statements (61.5%).
   Residual (b): the original strength-reduces the inner-loop start &row[i] as its own outer induction
   ([esp+0x14], +0x34 per pass) and keeps j in EBP; ours recomputes it each pass (imul) or derives it from ei;
   the x87 product order (original dy,dx,dz with fsubr for y/z) is the same for every operand order and
   temporaries form tried (6 orders x temps / no temps / V3 struct / declaration order), so it follows the
   register/schedule shape, not the expression. Tried: for/while/goto/pointer inner loops, 0- and 1-based outer
   counters, row pointer local, ej = ei + 1, separate ej0 induction, memcpy swap. */
/* ---- MATCH-S09 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
typedef struct Ent45ac40 {
    float m_pos[3];
    int m_0c[3];
    float m_18;
    int m_1c[5];
    int m_active;
} Ent45ac40;
typedef Ent45ac40 Grid45ac40[5][5];
MGLOBALA(Grid45ac40, DAT_0068bd00, 4, 0x0068bd00);

int FUN_004671d0(int p0);

// MATCH: 0x0045ac40 FUN_0045ac40
static void FUN_0045ac40(int a, int b, float *p)
{
    int i;
    int j;
    Ent45ac40 *ei;
    Ent45ac40 *ej;
    Ent45ac40 tmp;
    float dx, dy, dz, d;

    ei = DAT_0068bd00[a][b];
    for (i = 1; i < 6; i++, ei++) {
        if (ei->m_active != 0 && !(ei->m_18 >= 1.0f)) {
            dx = p[0] - ei->m_pos[0];
            dz = p[2] - ei->m_pos[2];
            dy = p[1] - ei->m_pos[1];
            d = dx * dx + dy * dy + dz * dz;
            j = i;
            if (j < 5) {
                ej = &DAT_0068bd00[a][b][j];
            top:
                if (ej->m_active != 0 && !(ej->m_18 >= 1.0f)) {
                    dx = p[0] - ej->m_pos[0];
                    dy = p[1] - ej->m_pos[1];
                    dz = p[2] - ej->m_pos[2];
                    if (dx * dx + dy * dy + dz * dz > d) {
                        tmp = *ej;
                        *ej = *ei;
                        *ei = tmp;
                    }
                }
                j++;
                ej++;
                if (j < 5) goto top;
            }
        }
    }
}

void Caller_0045ac40(void)
{
    int a;
    int b;

    for (a = 0; a < 4; a++) {
        for (b = 0; b < 5; b++) {
            FUN_0045ac40(a, b, (float *)FUN_004671d0(0));
        }
    }
}
